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

    int flags = fcntl(_serverFd, F_GETFL, 0);
    fcntl(_serverFd, F_SETFL, flags | O_NONBLOCK);

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
    }
    return *this;
}

void    Server::shutdown() {
    std::cout << "\nShutting down server..." << std::endl;

    for (size_t i = 0; i < _pollfds.size(); ++i) {
        close(_pollfds[i].fd);
    }
    _pollfds.clear();

    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }
    _clients.clear();
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
        throw std::runtime_error("accept");
    }

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
        // disconnect
        close(fd);
        _pollfds.erase(_pollfds.begin() + index);
        std::map<int, Client*>::iterator it = _clients.find(fd);
        if (it != _clients.end()) {
            delete it->second;
            _clients.erase(it);
        }
        std::cout << "Client disconnected." << std::endl;
        return;
    }
    buffer[bytes] = '\0';
    std::map<int, Client*>::iterator it = _clients.find(fd);
    if (it != _clients.end()) {
        Client *c = it->second;
        c->appendBuffer(buffer);
        std::cout << "Message from " << c->getHostname()
                  << ": " << c->getBuffer();
        c->clearBuffer();
    }
}