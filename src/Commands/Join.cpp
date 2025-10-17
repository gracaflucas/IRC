#include "../../include/Server.hpp"

std::map<std::string, std::string> parseJoinArgs(std::vector<std::string> &cmds) {
	std::map<std::string, std::string> map;
	std::vector<std::string> channels = splitString(cmds[1], ',');
	std::vector<std::string> keys;
	if (cmds.size() > 2)
		keys = splitString(cmds[2], ',');
	size_t i = 0;
	while (i < channels.size()) {
		if (i >= keys.size())
			map.insert(std::make_pair(channels[i], ""));
		else
			map.insert(std::make_pair(channels[i], keys[i]));
		i++;
	}
	return map;
}

void Server::joinCommand(std::vector<std::string> &cmds, Client *client) {
	if (!client->isAuth()) {
		sendResponse(client->getSocket(), ERR_NOTONCHANNEL(client->getNick(), "*"));
		return;
	}
	if (cmds.size() < 2) {
			sendResponse(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNick(), "JOIN"));
		return;
	}
	std::map<std::string, std::string> mapChannelKey = parseJoinArgs(cmds);
	for (std::map<std::string, std::string>::iterator it = mapChannelKey.begin(); 
		it != mapChannelKey.end(); ++it) {
		std::string channelName = it->first;
		std::string key = it->second;
		if (!isValidChannelName(channelName)) {
			sendResponse(client->getSocket(), ERR_BADCHANMASK(channelName));
			continue;
		}
		Channel *channel = this->getChannelByName(channelName);
		if (!channel) {
			Channel *newChannel = client->createChannel(channelName);
			this->_channels.insert(std::make_pair(newChannel->getName(), newChannel));
				client->sendToAllChannel(newChannel, RPL_JOIN(client->getPrefix(), newChannel->getName()));
			showNames(newChannel, client);
			sendResponse(client->getSocket(), RPL_NOTOPIC(client->getNick(), newChannel->getName()));
			continue;
		}
		if (client->isChannelMember(channel)) {
			continue;
		}
		if (channel->getUserLimit() != -1 && 
			static_cast<int>(channel->getClients().size()) >= channel->getUserLimit()) {
				sendResponse(client->getSocket(), ERR_CHANNELISFULL(client->getNick(), channel->getName()));
			continue;
		}
		if (channel->getInviteOnly()) {
			if (!client->isChannelInvited(channel)) {
					sendResponse(client->getSocket(), ERR_INVITEONLYCHAN(client->getNick(), channel->getName()));
				continue;
			}
		std::vector<int>::iterator invIt = std::find(
			channel->getChannelInvites().begin(),
			channel->getChannelInvites().end(),
			client->getSocket()
		);
		if (invIt != channel->getChannelInvites().end())
			channel->getChannelInvites().erase(invIt);
		}
		if (!channel->getKey().empty() && key != channel->getKey()) {
			sendResponse(client->getSocket(), ERR_BADCHANNELKEY(client->getNick(), channel->getName()));
				continue;
		}
		channel->addClient(client);
		client->sendToAllChannel(channel, RPL_JOIN(client->getPrefix(), channel->getName()));
		if (!channel->getTopic(0).empty()) {
				sendResponse(client->getSocket(), RPL_TOPIC(client->getNick(), channel->getName(), channel->getTopic(0)));
		} else {
			sendResponse(client->getSocket(), RPL_NOTOPIC(client->getNick(), channel->getName()));
		}
		showNames(channel, client);
	}
}