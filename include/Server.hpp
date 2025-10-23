#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <poll.h>
#include <algorithm>
#include <vector>
#include <map>
#include <sstream>
#include "Client.hpp"
#include "Channel.hpp"
#include "replies.hpp"
#include "utils.hpp"

class Client;
class Channel;

class Server {
    private:
        int                             _port;
        int                             _serverFd;
        std::string                     _password;
        std::vector<struct pollfd>      _pollfds;
        std::map<int, Client *>         _clients;
        std::map<std::string, Channel*> _channels;
        struct sockaddr_in              _address;

    public:
        Server();
        Server(int port, const std::string &password);
        Server(const Server& copy);
        ~Server();

        Server& operator=(const Server& other);

        void        run();
        void        acceptNewClient();
        void        handleClientMessage(size_t index);
        void        shutdown();
        void        tryAuthenticate(Client* client, const std::string& msg);
		
		void 		partCommand(std::vector<std::string> &cmds, Client *client);
        void        joinCommand(std::vector<std::string> &cmds, Client *client);
    	void		privmsgCommand(std::vector<std::string> &cmds, Client *client);
        void        inviteCommand(const std::vector<std::string>& cmd, Client *inviter);
        void        kickCommand(std::vector<std::string> &params, Client* kicker);
        
        Channel*    getChannelByName(const std::string &name);
        Client*     getClientBySocket(int socket);
		Client* 	getClientByNick(const std::string& nick);
		void        showNames(Channel *channel, Client *client);
        void        broadcastToChannel(Channel *channel, const std::string &message);
	};

#endif