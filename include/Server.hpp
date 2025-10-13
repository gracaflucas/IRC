#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <cstring>
#include <unistd.h> // close, lseek
#include <sys/stat.h> // fstat
#include <sys/socket.h> // socket, setsockopt, getsockname, bind, connect, listen, accept, send, recv
#include <netdb.h> // getprotobyname, gethostbyname, getadrrinfo, freeadrrinfo
#include <arpa/inet.h> // htons, htonl, ntohs, ntohl, inet_adrr, inet_ntoa
#include <signal.h> // signal, sigaction
#include <fcntl.h> // fcntl
#include <poll.h> // poll
#include <algorithm>
#include <vector>
#include <map>
#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;

class Server {
    private:
        int                         _port;
        int                         _serverFd;
        std::string                 _password;
        std::vector<struct pollfd>  _pollfds;
        std::map<int, Client *>     _clients;
        struct sockaddr_in          _address;
     //   bool                        _isShuttingDown;

    public:
        Server();
        Server(int port, const std::string &password);
        Server(const Server& copy);
        ~Server();

        Server& operator=(const Server& other);

        void    run();
        void    acceptNewClient();
        void    handleClientMessage(size_t index);
        void    shutdown();
};

#endif
