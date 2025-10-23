#include "../../include/Server.hpp"

void Server::kickCommand(std::vector<std::string> &params, Client* kicker) {
    if (params.size() < 3) {
        sendResponse(kicker->getSocket(), ERR_NEEDMOREPARAMS(kicker->getNick(), "KICK"));
        return;
    }
    std::string channelName = params[1];
    std::string targetNick  = params[2];
    std::string reason = "Kicked";
    if (params.size() > 3) {
        reason = "";
        for (size_t i = 3; i < params.size(); ++i) {
            if (i > 3) reason += " ";
            reason += params[i];
        }
        if (!reason.empty() && reason[0] == ':')
            reason = reason.substr(1);
    }
    Channel* channel = getChannelByName(channelName);
    if (!channel) {
        sendResponse(kicker->getSocket(), ERR_NOSUCHCHANNEL(kicker->getNick(), channelName));
        return;
    }
    if (!kicker->isChannelMember(channel)) {
        sendResponse(kicker->getSocket(), ERR_NOTONCHANNEL(kicker->getNick(), channelName));
        return;
    }
    if (!kicker->isChannelAdmin(channel)) {
        sendResponse(kicker->getSocket(), ERR_CHANOPRIVSNEEDED(kicker->getNick(), channelName));
        return;
    }
    Client* target = getClientByNick(targetNick);
    if (!target) {
        sendResponse(kicker->getSocket(), ERR_USERNOTINCHANNEL(kicker->getNick(), targetNick, channelName));
        return;
    }
    if (!target->isChannelMember(channel)) {
        sendResponse(kicker->getSocket(), ERR_USERNOTINCHANNEL(kicker->getNick(), targetNick, channelName));
        return;
    }
    std::string kickMsg = ":" + kicker->getPrefix() + " KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
    broadcastToChannel(channel, kickMsg);
    channel->removeClient(target->getSocket());
    if (target->isChannelAdmin(channel)) {
        channel->removeAdmin(target->getSocket());
    }
}
