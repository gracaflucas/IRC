#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include <algorithm>
#include <iostream> // debug

Channel::Channel(const std::string &name)
    : _name(name), _isInviteOnly(false), _isTopicRestricted(false), _maxUsers(0) {}

Channel::~Channel() {}

const std::string& Channel::getName() const {
    return _name;
}

const std::vector<Client*>& Channel::getMembers() const {
    return _members;
}

bool Channel::isInviteOnly() const {
    return _isInviteOnly;
}

bool Channel::isTopicRestricted() const {
    return _isTopicRestricted;
}

int Channel::getMaxUsers() const {
    return _maxUsers;
}

bool Channel::hasMember(Client* client) const {
    std::vector<Client*>::const_iterator it;
    for (it = _members.begin(); it != _members.end(); ++it) {
        if (*it == client)
            return true;
    }
    return false;
}

void Channel::addMember(Client* client) {
    if (hasMember(client))
        return;

    // Checa modos
    if (_isInviteOnly && !isInvited(client))
        return; // rejeita se canal +i

    if (_maxUsers != 0 && _members.size() >= static_cast<size_t>(_maxUsers))
        return; // rejeita se canal cheio

    _members.push_back(client);
}

void Channel::removeMember(Client* client) {
    std::vector<Client*>::iterator it;
    for (it = _members.begin(); it != _members.end(); ++it) {
        if (*it == client) {
            _members.erase(it);
            return;
        }
    }
}

void Channel::grantOperator(Client* client) {
    if (!isOperator(client))
        _operators.push_back(client);
}

void Channel::revokeOperator(Client* client) {
    std::vector<Client*>::iterator it;
    for (it = _operators.begin(); it != _operators.end(); ++it) {
        if (*it == client) {
            _operators.erase(it);
            return;
        }
    }
}

bool Channel::isOperator(Client* client) const {
    std::vector<Client*>::const_iterator it;
    for (it = _operators.begin(); it != _operators.end(); ++it) {
        if (*it == client)
            return true;
    }
    return false;
}

void Channel::inviteMember(Client* client) {
    if (!isInvited(client))
        _invited.push_back(client);
}

bool Channel::isInvited(Client* client) const {
    std::vector<Client*>::const_iterator it;
    for (it = _invited.begin(); it != _invited.end(); ++it) {
        if (*it == client)
            return true;
    }
    return false;
}

void Channel::sendMessageToAll(const std::string &msg, Client* sender) {
    std::vector<Client*>::iterator it;
    for (it = _members.begin(); it != _members.end(); ++it) {
        if (*it != sender)
            (*it)->sendMsgToClient(*it, msg);
    }
}
