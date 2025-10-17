#include "../../include/Server.hpp"

// Helper function to parse JOIN arguments
std::map<std::string, std::string> parseJoinArgs(std::vector<std::string> &cmds) {
    std::map<std::string, std::string> map;
    
    std::vector<std::string> channels = splitString(cmds[1], ',');
    std::vector<std::string> keys;
    
    if (cmds.size() > 2)
        keys = splitString(cmds[2], ',');
    
    for (size_t i = 0; i < channels.size(); i++) {
        std::string channel = channels[i];
        std::string key = (i < keys.size()) ? keys[i] : "";
        
        // Skip empty channel names
        if (!channel.empty())
            map.insert(std::make_pair(channel, key));
    }
    
    return map;
}

void Server::joinCommand(std::vector<std::string> &cmds, Client *client) {
    // Check if client is authenticated
    if (!client->isAuth()) {
        sendResponse(client->getSocket(), ERR_NOTONCHANNEL(client->getNick(), "*"));
        return;
    }
    
    // Check for parameters
    if (cmds.size() < 2) {
        sendResponse(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNick(), "JOIN"));
        return;
    }
    
    std::map<std::string, std::string> mapChannelKey = parseJoinArgs(cmds);
    
    for (std::map<std::string, std::string>::iterator it = mapChannelKey.begin(); 
         it != mapChannelKey.end(); ++it) {
        
        std::string channelName = it->first;
        std::string key = it->second;
        
        // Validate channel name
        if (!isValidChannelName(channelName)) {
            sendResponse(client->getSocket(), ERR_BADCHANMASK(channelName));
            continue;
        }
        
        Channel *channel = this->getChannelByName(channelName);
        
        // CASE 1: Channel doesn't exist - create new channel
        if (!channel) {
            Channel *newChannel = client->createChannel(channelName);
            this->_channels.insert(std::make_pair(newChannel->getName(), newChannel));
            
            // Broadcast JOIN to all members (including creator)
            client->sendToAllChannel(newChannel, RPL_JOIN(client->getPrefix(), newChannel->getName()));
            
            // Send NAMES list only to the joiner
            showNames(newChannel, client);
            
            // Send topic (new channels have no topic)
            sendResponse(client->getSocket(), RPL_NOTOPIC(client->getNick(), newChannel->getName()));
            
            continue;
        }
        
        // CASE 2: Channel exists - check if already member
        if (client->isChannelMember(channel)) {
            // Silently ignore (IRC standard behavior)
            continue;
        }
        
        // CASE 3: Check channel restrictions
        
        // Check user limit (+l mode)
        if (channel->getUserLimit() != -1 && 
            static_cast<int>(channel->getClients().size()) >= channel->getUserLimit()) {
            sendResponse(client->getSocket(), ERR_CHANNELISFULL(client->getNick(), channel->getName()));
            continue;
        }
        
        // Check invite-only (+i mode)
        if (channel->getInviteOnly()) {
            if (!client->isChannelInvited(channel)) {
                sendResponse(client->getSocket(), ERR_INVITEONLYCHAN(client->getNick(), channel->getName()));
                continue;
            }
            
            // Remove from invite list after successful join
            std::vector<int>::iterator invIt = std::find(
                channel->getChannelInvites().begin(),
                channel->getChannelInvites().end(),
                client->getSocket()
            );
            if (invIt != channel->getChannelInvites().end())
                channel->getChannelInvites().erase(invIt);
        }
        
        // Check channel key (+k mode)
        if (!channel->getKey().empty() && key != channel->getKey()) {
            sendResponse(client->getSocket(), ERR_BADCHANNELKEY(client->getNick(), channel->getName()));
            continue;
        }
        
        // CASE 4: All checks passed - add client to channel
        channel->addClient(client);
        
        // Broadcast JOIN to all members (including the joiner)
        client->sendToAllChannel(channel, RPL_JOIN(client->getPrefix(), channel->getName()));
        
        // Send topic to joiner only
        if (!channel->getTopic(0).empty()) {
            sendResponse(client->getSocket(), RPL_TOPIC(client->getNick(), channel->getName(), channel->getTopic(0)));
        } else {
            sendResponse(client->getSocket(), RPL_NOTOPIC(client->getNick(), channel->getName()));
        }
        
        // Send NAMES list to joiner only
        showNames(channel, client);
    }
}

void Server::showNames(Channel *channel, Client *client) {
    std::string names;
    std::vector<int> clients = channel->getClients();
    
    for (std::vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
        Client *cl = this->getClientBySocket(*it);
        if (!cl)
            continue;
        
        // Prefix operators with @
        if (cl->isChannelAdmin(channel))
            names += "@";
        
        names += cl->getNick();
        
        // Add space between nicknames (but not after last one)
        if (it != clients.end() - 1)
            names += " ";
    }
    
    // Send NAMES reply only to the requesting client
    sendResponse(client->getSocket(), RPL_NAMREPLY(client->getNick(), channel->getName(), names));
    sendResponse(client->getSocket(), RPL_ENDOFNAMES(client->getNick(), channel->getName()));
}