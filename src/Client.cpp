#include "../include/Client.hpp"

Client::Client(int socket, std::string &hostname) : _socket(socket), _isAuth(false), _insertPass(false), _hostName(hostname) {}

Client::~Client()
{
	std::vector<Channel*> &clientChannels = this->getChannels();
	std::vector<Channel*>::iterator it = clientChannels.begin();
	while (it != clientChannels.end()) {
		Channel* channel = *it;
		channel->removeClient(this->getSocket());
		if (this->isChannelAdmin(channel)) {
			channel->removeAdmin(this->getSocket());
		}
		++it;
	}
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

Channel *Client::createChannel(const std::string &name)
{
	Channel *c = new Channel(name);
	c->addClient(this);
	c->addAdmin(this);
	this->_channels.push_back(c);
	return c;
}

void	Client::sendMsgToClient(Client *client, const std::string &msg) {
		send(client->getSocket(), msg.c_str(), msg.size(), 0);
}
void Client::sendErrorMessage(const std::string &msg)
{
		send(_socket, msg.c_str(), msg.size(), 0);
}

bool	Client::isChannelAdmin(Channel *channel) {
	std::vector<int> admins = channel->getAdmins();
	if (std::find(admins.begin(), admins.end(), this->getSocket()) != admins.end())
		return true;
	return false;
}

bool Client::isChannelMember(Channel *channel) {
    std::vector<Channel*>::iterator it = std::find(
        _channels.begin(), 
        _channels.end(), 
        channel
    );
    return (it != _channels.end());
}

bool Client::isChannelInvited(Channel *channel) {
    std::vector<int> &invites = channel->getChannelInvites();
    std::vector<int>::iterator it = std::find(
        invites.begin(), 
        invites.end(), 
        _socket
    );
    return (it != invites.end());
}

void Client::sendToAllChannel(Channel *channel, const std::string &msg) {
    std::vector<int> &clients = channel->getClients();
    for (std::vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
        send(*it, msg.c_str(), msg.length(), 0);
    }
}