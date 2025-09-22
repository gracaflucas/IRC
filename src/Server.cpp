#include "../include/Server.hpp"

Server::Server() : fd(-1) {}
Server::Server(int fd) : fd(fd) {} 
Server::Server(const Server& copy) : fd(copy.fd) {}
Server::~Server() {}

Server& Server::operator=(const Server& other) {
    if (this != &other) {
        this->fd = other.fd;
    }
    return *this;
}