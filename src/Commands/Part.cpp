//PART #canal                    → Sai do canal
//PART #canal :Razão de saída    → Sai com mensagem
//PART #chan1,#chan2             → Sai de múltiplos canais

#include "../../include/Server.hpp"

// PART #channel1,#channel2 :reason
// cmds[0] = "PART"
// cmds[1] = channel(s)
// cmds[2+] = optional reason

void Server::partCommand(std::vector<std::string> &cmds, Client *client) {
    // Error: Not enough parameters
    if (cmds.size() < 2) {
        sendResponse(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNick(), "PART"));
        return;
    }
    // Parse multiple channels (comma-separated)
    std::vector<std::string> channels = splitString(cmds[1], ',');
    // Reconstruct reason if provided (starts with :)
    std::string reason;
    if (cmds.size() > 2) {
        for (size_t i = 2; i < cmds.size(); i++) {
            if (i == 2 && cmds[i][0] == ':')
                reason += cmds[i].substr(1);
            else
                reason += cmds[i];
            
            if (i < cmds.size() - 1)
                reason += " ";
        }
    }
    // Process each channel
    for (size_t i = 0; i < channels.size(); i++) {
        std::string channelName = channels[i];
        // Validate channel name
        if (!isValidChannelName(channelName)) {
            sendResponse(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNick(), channelName));
            continue;
        }
        // Check if channel exists
        Channel* channel = getChannelByName(channelName);
        if (!channel) {
            sendResponse(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNick(), channelName));
            continue;
        }
        // Check if client is in the channel
        if (!client->isChannelMember(channel)) {
            sendResponse(client->getSocket(), ERR_NOTONCHANNEL(client->getNick(), channelName));
            continue;
        }
        // Broadcast PART to all members (including the leaving client)
        std::string partMsg;
        if (reason.empty()) {
            partMsg = RPL_PART(client->getPrefix(), channelName);
        } else {
            partMsg = RPL_PART_REASON(client->getPrefix(), channelName, reason);
        }
        client->sendToAllChannel(channel, partMsg);
		// Remove client from channel
        channel->removeClient(client->getSocket());
        // Remove channel from client's channel list
        std::vector<Channel*> &clientChannels = client->getChannels();
        std::vector<Channel*>::iterator it = std::find(clientChannels.begin(), clientChannels.end(), channel);
        if (it != clientChannels.end()) {
            clientChannels.erase(it);
        }
        // If client was an operator, remove operator status
        if (client->isChannelAdmin(channel)) {
            channel->removeAdmin(client->getSocket());
        }
        // LOG PART FIRST (before checking if empty)
        std::cout << "[PART] " << client->getNick() << " left " << channelName;
        if (!reason.empty())
            std::cout << " (" << reason << ")";
        std::cout << std::endl;
        // THEN check if channel is now empty and delete
        if (channel->getClients().empty()) {
            std::map<std::string, Channel*>::iterator chanIt = _channels.find(channelName);
            if (chanIt != _channels.end()) {
                delete chanIt->second;
                _channels.erase(chanIt);
                std::cout << "[CHANNEL] " << channelName << " deleted (empty)" << std::endl;
            }
        }
    }
}