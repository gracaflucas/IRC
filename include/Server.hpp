#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <unistd.h> // close, lseek
#include <sys/stat.h> // fstat
#include <sys/socket.h> // socket, setsockopt, getsockname, bind, connect, listen, accept, send, recv
#include <netdb.h> // getprotobyname, gethostbyname, getadrrinfo, freeadrrinfo
#include <arpa/inet.h> // htons, htonl, ntohs, ntohl, inet_adrr, inet_ntoa
#include <signal.h> // signal, sigaction
#include <fcntl.h> // fcntl
#include <poll.h> // poll

class Server {
    private:
        int fd;

    public:
        Server();
        Server(int fd);
        Server(const Server& copy);
        ~Server();

        Server& operator=(const Server& other);
};

#endif
