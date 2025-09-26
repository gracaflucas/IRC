#include "../include/Client.hpp"

Client::Client(int socket, const std::string &hostname)
    : _socket(socket), _hostname(hostname), _buffer(""), _isAuth(false) {}

Client::~Client()
{
    close(_socket);
}

int Client::getSocket() const { return _socket; }
const std::string &Client::getHostname() const { return _hostname; }
std::string &Client::getBuffer() { return _buffer; }
bool Client::isAuth() const { return _isAuth; }

void Client::setAuth(bool status) { _isAuth = status; }

void Client::appendBuffer(const std::string &str)
{
    _buffer += str;
}

void Client::clearBuffer()
{
    _buffer.clear();
}

void	Client::sendMsgToClient(Client *client, const std::string &msg) {
	send(client->getSocket(), msg.c_str(), msg.size(), 0);
}
void Client::sendErrorMessage(const std::string &msg)
{
    send(_socket, msg.c_str(), msg.size(), 0);
}
