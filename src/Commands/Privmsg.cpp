#include "../../include/Server.hpp"
// PRIVMSG <target> :<message>
// cmds[0] = "PRIVMSG"
// cmds[1] = target (#channel or nickname)
// cmds[2+] = message (may need reconstruction if it had spaces)

void Server::privmsgCommand(std::vector<std::string> &cmds, Client *client) {
	if (cmds.size() < 2) {
		sendResponse(client->getSocket(), ERR_NORECIPIENT(client->getNick(), "PRIVMSG"));
		return;
	}
	if (cmds.size() < 3) {
		sendResponse(client->getSocket(), ERR_NOTEXTTOSEND(client->getNick()));
		return;
	}
	std::string target = cmds[1];
	std::string message;
		for (size_t i = 2; i < cmds.size(); i++) {
		if (i == 2 && cmds[i][0] == ':')
			message += cmds[i].substr(1);
		else
			message += cmds[i];
		if (i < cmds.size() - 1)
			message += " ";
	}
	// Check if target is a channel (starts with #)
	if (target[0] == '#') {
		Channel* channel = getChannelByName(target);
		if (!channel) {
			sendResponse(client->getSocket(), ERR_NOSUCHNICK(client->getNick(), target));
				return;
		}
		if (!client->isChannelMember(channel)) {
				sendResponse(client->getSocket(), ERR_CANNOTSENDTOCHAN(client->getNick(), target));
			return;
		}
		std::string fullMsg = ":" + client->getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
		std::vector<int> &members = channel->getClients();
		for (std::vector<int>::iterator it = members.begin(); it != members.end(); ++it) {
			if (*it != client->getSocket()) {
				send(*it, fullMsg.c_str(), fullMsg.size(), 0);
			}
		}
		std::cout << "[CHANNEL] " << client->getNick() << " -> " << target << ": " << message << std::endl;
		}
	else {
		// Target is a user - direct message
		Client* recipient = getClientByNick(target);
		
		if (!recipient) {
			sendResponse(client->getSocket(), ERR_NOSUCHNICK(client->getNick(), target));
			return;
			}
		std::string fullMsg = ":" + client->getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
			send(recipient->getSocket(), fullMsg.c_str(), fullMsg.size(), 0);
		std::cout << "[PRIVATE] " << client->getNick() << " -> " << target << ": " << message << std::endl;
	}
}