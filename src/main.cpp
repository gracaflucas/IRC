#include <iostream>
#include <unistd.h> // close, lseek
#include <sys/stat.h> // fstat
#include <sys/socket.h> // socket, setsockopt, getsockname, bind, connect, listen, accept, send, recv
#include <netdb.h> // getprotobyname, gethostbyname, getadrrinfo, freeadrrinfo
#include <arpa/inet.h> // htons, htonl, ntohs, ntohl, inet_adrr, inet_ntoa
#include <csignal> // signal, sigaction
#include <fcntl.h> // fcntl
#include <poll.h> // poll
#include "../include/Server.hpp"
#include "../include/utils.hpp"


static Server *g_server = NULL;

static void handleSigint(int) {
    if (g_server) {
        std::cout << "\nShutting down server..." << std::endl;
        g_server->shutdown();
        delete g_server;
        g_server = NULL;
    }
    std::exit(0);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Try ./ircserv <port> <password>" << std::endl;
        return (1);
    }
    std::signal(SIGINT, handleSigint);
    try {
        int port = parsePort(argv[1]);
        std::string password = parsePassword(argv[2]);

        std::cout << "Starting IRC server on port " << port 
                  << " with password [" << password << "]" << std::endl;
        g_server = new Server(port, password);
        g_server->run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }
    return (0);
}

// #include "../include/Channel.hpp"
// #include "../include/Client.hpp"
// #include <iostream>

// int main() {
//     // Simular sockets apenas como números fictícios
//     Client alice(1, "alice_host");
//     Client bob(2, "bob_host");

//     Channel general("general");

//     general.addMember(&alice);
//     general.addMember(&bob);

//     // Alice envia mensagem
//     general.sendMessageToAll("Olá, pessoal!", &alice);

//     // Tornar Alice operador
//     general.grantOperator(&alice);
//     std::cout << "Alice é operador? " << general.isOperator(&alice) << std::endl;

//     return 0;
// }
