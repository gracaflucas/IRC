//PART #canal                    → Sai do canal
//PART #canal :Razão de saída    → Sai com mensagem
//PART #chan1,#chan2             → Sai de múltiplos canais

#include "../../include/Server.hpp"

// PART #channel1,#channel2 :reason
// cmds[0] = "PART"
// cmds[1] = channel(s)
// cmds[2+] = optional reason

void Server::partCommand(std::vector<std::string> &cmds, Client *client) {
    if (cmds.size() < 2) {
        sendResponse(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNick(), "PART"));
        return;
    }
    std::vector<std::string> channels = splitString(cmds[1], ',');
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
    for (size_t i = 0; i < channels.size(); i++) {
        std::string channelName = channels[i];
        if (!isValidChannelName(channelName)) {
            sendResponse(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNick(), channelName));
            continue;
        }
        Channel* channel = getChannelByName(channelName);
        if (!channel) {
            sendResponse(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNick(), channelName));
            continue;
        }
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
        channel->removeClient(client->getSocket());
        std::vector<Channel*> &clientChannels = client->getChannels();
        std::vector<Channel*>::iterator it = std::find(clientChannels.begin(), clientChannels.end(), channel);
        if (it != clientChannels.end()) {
            clientChannels.erase(it);
        }
        if (client->isChannelAdmin(channel)) {
            channel->removeAdmin(client->getSocket());
        }
        std::cout << "[PART] " << client->getNick() << " left " << channelName;
        if (!reason.empty())
            std::cout << " (" << reason << ")";
        std::cout << std::endl;
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