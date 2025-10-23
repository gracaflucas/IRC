/* MODE - Change the channel’s mode:
· i: Set/remove Invite-only channel
· t: Set/remove the restrictions of the TOPIC command to channel
operators
· k: Set/remove the channel key (password)
· o: Give/take channel operator privilege
· l: Set/remove the user limit to channel */

#include "../../include/Server.hpp"

// MODE #channel [+/-modes] [params...]
// cmds[0] = "MODE"
// cmds[1] = channel
// cmds[2] = mode string (optional)
// cmds[3+] = mode parameters (optional)

// Helper to check if string is a positive number
static bool isPositiveNumber(const std::string& str) {
    if (str.empty())
        return false;
    for (size_t i = 0; i < str.length(); i++) {
        if (!std::isdigit(str[i]))
            return false;
    }
    return true;
}

void Server::modeCommand(std::vector<std::string> &cmds, Client *client) {
    
    if (cmds.size() < 2) {
        sendResponse(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
        return;
    }
    std::string channelName = cmds[1];
    if (!isValidChannelName(channelName)) {
        sendResponse(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNick(), channelName));
        return;
    }
    Channel* channel = getChannelByName(channelName);
    if (!channel) {
        sendResponse(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNick(), channelName));
        return;
    }
    if (!client->isChannelMember(channel)) {
        sendResponse(client->getSocket(), ERR_NOTONCHANNEL(client->getNick(), channelName));
        return;
    }
    if (cmds.size() == 2) {
        std::string currentMode = "+";
        std::string modeParams = "";
        if (channel->getInviteOnly())
            currentMode += "i";
        if (channel->getTopicRestricted())
            currentMode += "t";
        if (!channel->getKey().empty()) {
            currentMode += "k";
            modeParams += " " + channel->getKey();
        }
        if (channel->getUserLimit() != -1) {
            currentMode += "l";
            std::ostringstream oss;
            oss << channel->getUserLimit();
            modeParams += " " + oss.str();
        }
        if (currentMode == "+")
            currentMode = "+";
        std::string response = ":server 324 " + client->getNick() + " " + channelName + " " + currentMode + modeParams + "\r\n";
        sendResponse(client->getSocket(), response);
        return;
    }
    // Check if client is channel operator
    if (!client->isChannelAdmin(channel)) {
        sendResponse(client->getSocket(), ERR_CHANOPRIVSNEEDED(client->getNick(), channelName));
        return;
    }
    std::string modeString = cmds[2];
    size_t paramIndex = 3;
    bool adding = true;
    std::string appliedModes = "";
    std::string appliedParams = "";
    for (size_t i = 0; i < modeString.length(); i++) {
        char mode = modeString[i];
        if (mode == '+') {
            adding = true;
            continue;
        }
        else if (mode == '-') {
            adding = false;
            continue;
        }
        switch (mode) {
            case 'i': // Invite-only
                channel->setInviteOnly(adding);
                appliedModes += (adding ? "+" : "-");
                appliedModes += "i";
                std::cout << "[MODE] " << channelName << (adding ? " +i" : " -i") << " (invite-only) by " << client->getNick() << std::endl;
                break;
            case 't': // Topic restricted
                channel->setTopicRestricted(adding);
                appliedModes += (adding ? "+" : "-");
                appliedModes += "t";
                std::cout << "[MODE] " << channelName << (adding ? " +t" : " -t") << " (topic restricted) by " << client->getNick() << std::endl;
                break;
            case 'k': // Channel key (password)
                if (adding) {
                    if (paramIndex >= cmds.size()) {
                        sendResponse(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNick(), "MODE +k"));
                        continue;
                    }
                    std::string key = cmds[paramIndex++];
                    channel->setKey(key);
                    appliedModes += "+k";
                    appliedParams += " " + key;
                    std::cout << "[MODE] " << channelName << " +k " << key << " (key set) by " << client->getNick() << std::endl;
                } else {
                    channel->setKey("");
                    appliedModes += "-k";
                    std::cout << "[MODE] " << channelName << " -k (key removed) by " << client->getNick() << std::endl;
                }
                break;
            case 'o': // Operator privilege
                if (paramIndex >= cmds.size()) {
                    sendResponse(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNick(), "MODE +/-o"));
                    continue;
                }
                {
                    std::string targetNick = cmds[paramIndex++];
                    Client* target = getClientByNick(targetNick);
                    
                    if (!target) {
                        sendResponse(client->getSocket(), ERR_NOSUCHNICK(client->getNick(), targetNick));
                        continue;
                    }
                    if (!target->isChannelMember(channel)) {
                        sendResponse(client->getSocket(), ERR_USERNOTINCHANNEL(client->getNick(), targetNick, channelName));
                        continue;
                    }
                    if (adding) {
                        if (!target->isChannelAdmin(channel)) {
                            channel->addAdmin(target);
                            appliedModes += "+o";
                            appliedParams += " " + targetNick;
                            std::cout << "[MODE] " << channelName << " +o " << targetNick << " by " << client->getNick() << std::endl;
                        }
                    } else {
                        if (target->isChannelAdmin(channel)) {
                            channel->removeAdmin(target->getSocket());
                            appliedModes += "-o";
                            appliedParams += " " + targetNick;
                            std::cout << "[MODE] " << channelName << " -o " << targetNick << " by " << client->getNick() << std::endl;
                        }
                    }
                }
                break;
            case 'l': // User limit
				if (adding) {
					if (paramIndex >= cmds.size()) {
						sendResponse(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNick(), "MODE +l"));
						continue;
					}
					std::string limitStr = cmds[paramIndex++];
					if (!isPositiveNumber(limitStr)) {
						continue;
					}
					int limit = std::atoi(limitStr.c_str());
					if (limit <= 0) {
						continue;
					}
					channel->setUserLimit(limit);
					appliedModes += "+l";
					appliedParams += " " + limitStr;
					std::cout << "[MODE] " << channelName << " +l " << limit << " (user limit) by " << client->getNick() << std::endl;
				} else {
					channel->setUserLimit(-1);
					appliedModes += "-l";
					std::cout << "[MODE] " << channelName << " -l (limit removed) by " << client->getNick() << std::endl;
				}
				break;
        }
    }
    // Broadcast MODE change to all channel members
    if (!appliedModes.empty()) {
        std::string modeMsg = ":" + client->getPrefix() + " MODE " + channelName + " " + appliedModes + appliedParams + "\r\n";
        broadcastToChannel(channel, modeMsg);
    }
}