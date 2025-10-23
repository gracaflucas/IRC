#include "../../include/Server.hpp"

void Server::inviteCommand(const std::vector<std::string>& cmd, Client *inviter) {
    if (cmd.size() < 3) {
        sendResponse(inviter->getSocket(), ERR_NEEDMOREPARAMS(inviter->getNick(), "INVITE"));
        return;
    }
    std::string targetNick = cmd[1];
    std::string channelName = cmd[2];

    Channel* channel = getChannelByName(channelName);
    if (!channel) {
        sendResponse(inviter->getSocket(), ERR_NOSUCHCHANNEL(inviter->getNick(), channelName));
        return;
    }

    std::vector<int>& members = channel->getClients();
    if (std::find(members.begin(), members.end(), inviter->getSocket()) == members.end()) {
        sendResponse(inviter->getSocket(), ERR_NOTONCHANNEL(inviter->getNick(), channelName));
        return;
    }

    if (channel->getInviteOnly() && !inviter->isChannelAdmin(channel)) {
        sendResponse(inviter->getSocket(), ERR_CHANOPRIVSNEEDED(inviter->getNick(), channelName));
        return;
    }

    Client* target = getClientByNick(targetNick);
    if (!target) {
        sendResponse(inviter->getSocket(), ERR_NOSUCHNICK(inviter->getNick(), targetNick));
        return;
    }

    channel->addChannelInvite(target);
    sendResponse(inviter->getSocket(), RPL_INVITING(inviter->getNick(), targetNick, channelName));
    std::string inviteMsg = ":" + inviter->getNick() + "!" + inviter->getUser() + "@localhost INVITE " + targetNick + " :" + channelName + "\r\n";
    sendResponse(target->getSocket(), inviteMsg);
}