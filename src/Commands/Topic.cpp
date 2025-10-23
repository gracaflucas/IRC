// preciso incluir no Makefile, e os metodos na Include SERVER
// testar com mais parametros do que os necessarios, incluindo topic, invite e kick
// tentar topic com multiplas palavras depois do :
// checar se ha algum topico quando o user pede, e nao ha topico
#include "../../include/Server.hpp"
#include <ctime>
#include <sstream>

static std::string joinParams(const std::vector<std::string>& params, size_t start) {
    std::string result;
    for (size_t i = start; i < params.size(); ++i) {
        if (i != start) result += " ";
        result += params[i];
    }
    return result;
}

void Server::topicCommand(std::vector<std::string> &cmds, Client *client) {
    if (cmds.size() < 2) {
        sendResponse(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNick(), "TOPIC"));
        return;
    }
    std::string channelName = cmds[1];
    Channel *channel = getChannelByName(channelName);
    if (!channel) {
        sendResponse(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNick(), channelName));
        return;
    }
    if (!client->isChannelMember(channel)) {
        sendResponse(client->getSocket(), ERR_NOTONCHANNEL(client->getNick(), channelName));
        return;
    }
    if (cmds.size() == 2) {
        std::string topic = channel->getTopic(0);
        if (topic.empty()) {
            sendResponse(client->getSocket(), RPL_NOTOPIC(client->getNick(), channelName));
        } else {
            sendResponse(client->getSocket(), RPL_TOPIC(client->getNick(), channelName, topic));
        }
        return;
    }
    if (channel->getTopicRestricted() && !client->isChannelAdmin(channel)) {
        sendResponse(client->getSocket(), ERR_CHANOPRIVSNEEDED(client->getNick(), channelName));
        return;
    }
    std::string newTopic = joinParams(cmds, 2);
    if (!newTopic.empty() && newTopic[0] == ':') {
        newTopic = newTopic.substr(1);
    }
    std::time_t now = std::time(0);
    std::ostringstream timeStr;
    timeStr << now;
    channel->setTopic(newTopic, client->getNick(), timeStr.str());
    std::string msg = ":" + client->getPrefix() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
    broadcastToChannel(channel, msg);
}