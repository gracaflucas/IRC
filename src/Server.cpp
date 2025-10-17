#include "../include/Server.hpp"

Server::Server() : _port(-1), _serverFd(-1), _password("") {}

Server::Server(int port, const std::string &password) : _port(port), _serverFd(-1), _password(password) {
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0) {
        throw std::runtime_error("Failed to create socket.");
    }

    int opt = 1;
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(_serverFd);
        throw std::runtime_error("Failed to set socket options.");
    }

    std::memset(&_address, 0, sizeof(_address));
    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(_port);

    if (bind(_serverFd, (struct sockaddr*)&_address, sizeof(_address)) < 0) {
        close(_serverFd);
        throw std::runtime_error("Bind failed.");
    }

    if (listen(_serverFd, SOMAXCONN) < 0) {
        close(_serverFd);
        throw std::runtime_error("Listen failed.");
    }
    fcntl(_serverFd, F_SETFL, O_NONBLOCK);

    std::cout << "Server listening on port " << _port << std::endl;
} 

Server::Server(const Server& copy) : _port(copy._port), _serverFd(-1), _password(copy._password) {}

Server::~Server() {
    shutdown();
}

Server& Server::operator=(const Server& other) {
    if (this != &other) {
        this->_port = other._port;
        this->_serverFd = -1;
        this->_password = other._password;
        this->_clients.clear();
        this->_channels.clear();
    }
    return *this;
}

void Server::shutdown() {
    for (size_t i = 0; i < _pollfds.size(); ++i) {
        close(_pollfds[i].fd);
    }
    _pollfds.clear();

    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }
    _clients.clear();
    
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        delete it->second;
    }
    _channels.clear();
}

void Server::run() {
    struct pollfd serverPoll;
    serverPoll.fd = _serverFd;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;
    _pollfds.push_back(serverPoll);

    while (true) {
        int activity = poll(&_pollfds[0], _pollfds.size(), -1);
        if (activity < 0) {
            throw std::runtime_error("poll() failed.");
        }
        for (size_t i = 0; i < _pollfds.size(); i++) {
            if (_pollfds[i].revents & POLLIN) {
                if (_pollfds[i].fd == _serverFd) {
                    acceptNewClient();
                } else {
                    handleClientMessage(i);
                }
            }
        }
    }
}

void Server::acceptNewClient() {
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(_serverFd, (sockaddr*)&clientAddr, &clientLen);
    if (clientFd < 0) {
        std::cerr << "Warning: accept() failed" << std::endl;
        return;
    }
    fcntl(clientFd, F_SETFL, O_NONBLOCK);

    std::string hostname = inet_ntoa(clientAddr.sin_addr);
    Client *newClient = new Client(clientFd, hostname);
    _clients[clientFd] = newClient;

    struct pollfd clientPoll;
    clientPoll.fd = clientFd;
    clientPoll.events = POLLIN;
    clientPoll.revents = 0;
    _pollfds.push_back(clientPoll);

    std::cout << "New client connected: FD=" << clientFd
              << " Host=" << hostname << std::endl;
}

void Server::handleClientMessage(size_t index) {
    int fd = _pollfds[index].fd;
    char buffer[512];
    int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0) {
        close(fd);
        _pollfds.erase(_pollfds.begin() + index);
        
        std::map<int, Client*>::iterator it = _clients.find(fd);
        if (it != _clients.end()) {
            std::cout << "Client disconnected: " << it->second->getNick() << std::endl;
            delete it->second;
            _clients.erase(it);
        }
        return;
    }
    
    buffer[bytes] = '\0';
    std::map<int, Client*>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return;
    
    Client *c = it->second;
    c->appendBuffer(buffer);
    
    std::string &buf = c->getBuffer();
    size_t pos;
    while ((pos = buf.find("\r\n")) != std::string::npos) {
        std::string command = buf.substr(0, pos);
        buf.erase(0, pos + 2);
        
        if (command.empty())
            continue;
        
        if (!c->isAuth()) {
            tryAuthenticate(c, command);
        } else {
            std::istringstream iss(command);
            std::vector<std::string> cmds;
            std::string word;
            
            while (iss >> word)
                cmds.push_back(word);
            
            if (cmds.empty())
                continue;
            
            if (cmds[0] == "JOIN") {
                joinCommand(cmds, c);
            }
            else {
                sendResponse(c->getSocket(), ERR_UNKNOWNCOMMAND(c->getNick(), cmds[0]));
            }
        }
    }
}

void Server::tryAuthenticate(Client* client, const std::string& msg) {
    std::string line = msg;
    
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);
    
    if (line.find("PASS ") == 0) {
        std::string pass = line.substr(5);
        if (pass == _password) {
            client->setInsertPass(true);
            sendResponse(client->getSocket(), ":server NOTICE * :Password accepted\r\n");
        } else {
            sendResponse(client->getSocket(), ERR_PASSWDMISMATCH("*"));
        }
    }
    else if (line.find("NICK ") == 0) {
        std::string nick = line.substr(5);
        
        if (nick.empty()) {
            sendResponse(client->getSocket(), ERR_NONICKNAMEGIVEN("*"));
            return;
        }
        
        bool taken = false;
        for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
            if (it->second->getNick() == nick) {
                taken = true;
                break;
            }
        }
        
        if (taken) {
            sendResponse(client->getSocket(), ERR_NICKNAMEINUSE("*", nick));
        } else {
            client->setNick(nick);
            sendResponse(client->getSocket(), ":server NOTICE * :Nickname set\r\n");
        }
    }
    else if (line.find("USER ") == 0) {
        std::istringstream parts(line.substr(5));
        std::string user, unused1, unused, realName;
        parts >> user >> unused1 >> unused;
        std::getline(parts, realName);
        
        if (user.empty() || unused1.empty() || unused.empty() || realName.empty() || 
            realName.length() < 2 || realName[1] != ':') {
            sendResponse(client->getSocket(), ERR_NEEDMOREPARAMS("*", "USER"));
            return;
        }
        
        realName = realName.substr(2);
        client->setUser(user);
        client->setRealName(realName);
        sendResponse(client->getSocket(), ":server NOTICE * :User registered\r\n");
    }
    else {
        std::string cmd = line.substr(0, line.find(' '));
        std::string nick = client->getNick().empty() ? "*" : client->getNick();
        sendResponse(client->getSocket(), ERR_UNKNOWNCOMMAND(nick, cmd));
    }
    
    if (!client->isAuth() && client->insertedPass() && 
        !client->getNick().empty() && !client->getUser().empty()) {
        client->setAuth(true);
        sendResponse(client->getSocket(), RPL_WELCOME(client->getNick()));
        std::cout << "Client " << client->getNick() << " authenticated successfully." << std::endl;
    }
}

Channel* Server::getChannelByName(const std::string &name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end())
        return it->second;
    return NULL;
}

Client* Server::getClientBySocket(int socket) {
    std::map<int, Client*>::iterator it = _clients.find(socket);
    if (it != _clients.end())
        return it->second;
    return NULL;
}

void Server::showNames(Channel *channel, Client *client) {
    std::string names;
    std::vector<int> clients = channel->getClients();
    
    for (std::vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
        Client *cl = this->getClientBySocket(*it);
        if (!cl)
            continue;
        
        if (cl->isChannelAdmin(channel))
            names += "@";
        
        names += cl->getNick();
        
        if (it != clients.end() - 1)
            names += " ";
    }
    
    sendResponse(client->getSocket(), RPL_NAMREPLY(client->getNick(), channel->getName(), names));
    sendResponse(client->getSocket(), RPL_ENDOFNAMES(client->getNick(), channel->getName()));
}