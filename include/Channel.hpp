#pragma once
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <vector>

class Client; 
class Server; 

class Channel
{
private:
    std::string _name;                   // Nome do canal
    std::vector<Client*> _members;       // Todos os clientes que entraram
    std::vector<Client*> _operators;     // Clientes com permissões de admin
    std::vector<Client*> _invited;       // Lista de convidados

    // Modos do canal
    bool _isInviteOnly;                  // +i: apenas convidados podem entrar
    bool _isTopicRestricted;             // +t: apenas operadores podem mudar topic
    int  _maxUsers;                      // +l: limite de usuários

public:
    Channel(const std::string &name);
    ~Channel();

    // Getters
    const std::string& getName() const;
    const std::vector<Client*>& getMembers() const;

    // Membership
    void addMember(Client* client);
    void removeMember(Client* client);
    bool hasMember(Client* client) const;

    // Operator management
    void grantOperator(Client* client);
    void revokeOperator(Client* client);
    bool isOperator(Client* client) const;

    // Invites
    void inviteMember(Client* client);
    bool isInvited(Client* client) const;

    // Minimal getters for modes to avoid warnings
    bool isInviteOnly() const;
    bool isTopicRestricted() const;
    int  getMaxUsers() const;

    // Messaging
    void sendMessageToAll(const std::string &msg, Client* sender = 0);
};

#endif
