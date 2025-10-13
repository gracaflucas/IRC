#include "../include/Client.hpp"

Client::Client(int socket, const std::string &hostname)
    : _socket(socket), _hostName(hostname), _buffer(""), _isAuth(false) {}

Client::~Client()
{
    close(_socket);
}

//***********GETTERS***************/
int     Client::getSocket() const {
    return this->_socket;
}

bool    Client::insertedPass() const {
    return this->_insertPass;
}

std::string     Client::getUser() const {
    return this->_user;
}

std::string     Client::getNick() const {
    return this->_nick;
}

std::string		Client::getRealName() const {
	return this->_realName;
}

std::string		Client::getHostname() const {
	return this->_hostName;
}

std::vector<Channel *>& Client::getChannels() {
	return this->_channels;
}

const std::string Client::getPrefix() const {
	return _nick + "!" + _user + "@" + _hostName;
}

std::string& Client::getBuffer() {
	return this->_buffer;
}


// **************SETTERS********************
void    Client::setInsertPass(bool status) {
		_insertPass = status;
}

void    Client::setAuth(bool status) {
		_isAuth = status;
}

void    Client::setNick(const std::string &nick) {
		this->_nick = nick;
}

void    Client::setUser(const std::string &user) {
		this->_user = user;
}

void	Client::setRealName(const std::string &realName) {
		this->_realName = realName;
}

bool    Client::isAuth() const {
		return this->_isAuth;
}

void Client::appendBuffer(const std::string &str)
{
		_buffer += str;
}

void Client::clearBuffer()
{
		this->_buffer.clear();
}

void	Client::sendMsgToClient(Client *client, const std::string &msg) {
		send(client->getSocket(), msg.c_str(), msg.size(), 0);
}
void Client::sendErrorMessage(const std::string &msg)
{
		send(_socket, msg.c_str(), msg.size(), 0);
}
