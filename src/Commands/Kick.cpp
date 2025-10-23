#include "../../include/Server.hpp"

void Server::kickCommand(std::vector<std::string> &params, Client* kicker) {
    if (params.size() < 3) {
        sendResponse(kicker->getSocket(), ERR_NEEDMOREPARAMS(kicker->getNick(), "KICK"));
        return;
    }

    std::string channelName = params[1];
    std::string targetNick  = params[2];
    std::string reason = (params.size() > 3) ? params[3] : "Kicked";
    Channel* channel = getChannelByName(channelName);
    if (!channel) {
        sendResponse(kicker->getSocket(), ERR_NOSUCHCHANNEL(kicker->getNick(), channelName));
        return;
    }
    std::vector<int>& members = channel->getClients();
    if (std::find(members.begin(), members.end(), kicker->getSocket()) == members.end()) {
        sendResponse(kicker->getSocket(), ERR_NOTONCHANNEL(kicker->getNick(), channelName));
        return;
    }

    if (!kicker->isChannelAdmin(channel)) {
        sendResponse(kicker->getSocket(), ERR_CHANOPRIVSNEEDED(kicker->getNick(), channelName));
        return;
    }

    Client* target = getClientByNick(targetNick);
    if (!target || std::find(members.begin(), members.end(), target->getSocket()) == members.end()) {
        sendResponse(kicker->getSocket(), ERR_USERNOTINCHANNEL(kicker->getNick(), targetNick, channelName));
        return;
    }

    std::string kickMsg = ":" + kicker->getNick() + "!" + kicker->getUser() + "@localhost KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
    broadcastToChannel(channel, kickMsg);
    channel->removeClient(target->getSocket());
    sendResponse(target->getSocket(), kickMsg);
}

void Server::broadcastToChannel(Channel *channel, const std::string &message) {
    if (!channel)
        return;
    std::vector<int> clients = channel->getClients();
    for (std::vector<int>::size_type i = 0; i < clients.size(); ++i) {
        Client *c = getClientBySocket(clients[i]);
        if (c)
            sendResponse(c->getSocket(), message);
    }
}
