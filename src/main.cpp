#include <iostream>
#include <unistd.h> // close, lseek
#include <sys/stat.h> // fstat
#include <sys/socket.h> // socket, setsockopt, getsockname, bind, connect, listen, accept, send, recv
#include <netdb.h> // getprotobyname, gethostbyname, getadrrinfo, freeadrrinfo
#include <arpa/inet.h> // htons, htonl, ntohs, ntohl, inet_adrr, inet_ntoa
#include <signal.h> // signal, sigaction
#include <fcntl.h> // fcntl
#include <poll.h> // poll
#include "./include/Server.hpp"
#include "./include/utils.hpp"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Try ./ircserv <port> <password>" << std::endl;
        return (1);
    }

    // validate arguments
    // port is at least 1 character (digit), between 1 and 65535 (16 bits unsigned)
    // password is at least 1 character, can contain any printable characters

    // int port = parsePort(argv[1]);
    // std::string password = parsePassword(argv[2]);

    // std::cout << "Starting IRC server on port " << port 
    //           << " with password [" << password << "]" << std::endl;

    return (0);
}