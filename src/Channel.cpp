#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include <algorithm>
#include <iostream> // debug

Channel::Channel(const std::string &name): _name(name), _key(""), _inviteOnly(false), _topicRestricted(false), _userLimit(-1){}

Channel::~Channel() {}

std::string	Channel::getName() const {
	return this->_name;
}

std::string	Channel::getTopic(unsigned int index) const {
	return this->_topic[index];
}

std::string	Channel::getKey() const {
	return this->_key;
}

std::string	Channel::getMode() const {
	return this->_mode;
}

std::vector<int> &Channel::getClients() {
	return this->_channelClients;
}

std::vector<int> &Channel::getAdmins() {
	return this->_channelAdmins;
}

std::vector<int> &Channel::getChannelInvites() {
	return this->_channelInvites;
}


int	Channel::getUserLimit() const {
	return this->_userLimit;
}

bool	Channel::getInviteOnly() const {
	return this->_inviteOnly;
}

bool	Channel::getTopicRestricted() const {
	return this->_topicRestricted;
}

void	Channel::setName(const std::string &newName) {
	this->_name = newName;
}

void	Channel::setTopic(const std::string &newTopic, std::string client, std::string time) {
	this->_topic[0] = newTopic;
	this->_topic[1] = client;
	this->_topic[2] = time;
}

void	Channel::setKey(const std::string &newKey) {
	this->_key = newKey;
}

void	Channel::setMode(const std::string &newMode) {
	this->_mode = newMode;
}

void	Channel::setInviteOnly(bool status) {
	this->_inviteOnly = status;
}

void	Channel::setTopicRestricted(bool status) {
	this->_topicRestricted = status;
}

void	Channel::setUserLimit(int limit) {
	this->_userLimit = limit;
}

void	Channel::addClient(Client *client) {
	this->_channelClients.push_back(client->getSocket());
	client->getChannels().push_back(this);
}

void	Channel::addAdmin(Client *client) {
	this->_channelAdmins.push_back(client->getSocket());
}

void	Channel::removeClient(int socket) {
	std::vector<int>::iterator it = std::find(_channelClients.begin(), _channelClients.end(), socket);
	if (it != _channelClients.end())
		_channelClients.erase(it);
}

void	Channel::removeAdmin(int socket) {
	std::vector<int>::iterator it = std::find(_channelAdmins.begin(), _channelAdmins.end(), socket);
	if (it != _channelAdmins.end())	
		_channelAdmins.erase(it);
}

void	Channel::addChannelInvite(const Client *client){
	this->_channelInvites.push_back(client->getSocket());
};
