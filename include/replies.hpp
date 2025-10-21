#ifndef REPLIES_HPP
#define REPLIES_HPP

#include <string>
#include <sstream>

// ============================================================================
// RPL - Successful Command Replies
// ============================================================================

// 001 - Welcome message
#define RPL_WELCOME(nick) \
    (std::string(":server 001 ") + nick + " :Welcome to the IRC server!\r\n")

// 331 - No topic set
#define RPL_NOTOPIC(nick, channel) \
    (std::string(":server 331 ") + nick + " " + channel + " :No topic is set\r\n")

// 332 - Channel topic
#define RPL_TOPIC(nick, channel, topic) \
    (std::string(":server 332 ") + nick + " " + channel + " :" + topic + "\r\n")

// 353 - Names reply (list of users in channel)
#define RPL_NAMREPLY(nick, channel, names) \
    (std::string(":server 353 ") + nick + " = " + channel + " :" + names + "\r\n")

// 366 - End of names list
#define RPL_ENDOFNAMES(nick, channel) \
    (std::string(":server 366 ") + nick + " " + channel + " :End of /NAMES list\r\n")

// JOIN - User joins channel (uses client prefix: nick!user@host)
#define RPL_JOIN(prefix, channel) \
    (std::string(":") + prefix + " JOIN " + channel + "\r\n")

// PART - User leaves channel
#define RPL_PART(prefix, channel) \
    (std::string(":") + prefix + " PART " + channel + "\r\n")

// PART with reason
#define RPL_PART_REASON(prefix, channel, reason) \
    (std::string(":") + prefix + " PART " + channel + " :" + reason + "\r\n")

// KICK - User kicked from channel
#define RPL_KICK(prefix, channel, target, reason) \
    (std::string(":") + prefix + " KICK " + channel + " " + target + " :" + reason + "\r\n")

// INVITE - User invited to channel
#define RPL_INVITE(prefix, target, channel) \
    (std::string(":") + prefix + " INVITE " + target + " " + channel + "\r\n")

// 341 - Invite confirmation
#define RPL_INVITING(nick, target, channel) \
    (std::string(":server 341 ") + nick + " " + target + " " + channel + "\r\n")

// TOPIC - Topic change broadcast
#define RPL_TOPICCHANGE(prefix, channel, topic) \
    (std::string(":") + prefix + " TOPIC " + channel + " :" + topic + "\r\n")

// MODE - Mode change broadcast
#define RPL_MODE(prefix, channel, modes, args) \
    (std::string(":") + prefix + " MODE " + channel + " " + modes + " " + args + "\r\n")

// PRIVMSG - Private message
#define RPL_PRIVMSG(prefix, target, message) \
    (std::string(":") + prefix + " PRIVMSG " + target + " :" + message + "\r\n")

// NOTICE - Notice message
#define RPL_NOTICE(prefix, target, message) \
    (std::string(":") + prefix + " NOTICE " + target + " :" + message + "\r\n")

// QUIT - User quit
#define RPL_QUIT(prefix, reason) \
    (std::string(":") + prefix + " QUIT :" + reason + "\r\n")

// ============================================================================
// ERR - Error Replies
// ============================================================================

// 401 - No such nick/channel
#define ERR_NOSUCHNICK(nick, target) \
    (std::string(":server 401 ") + nick + " " + target + " :No such nick/channel\r\n")

// 403 - No such channel
#define ERR_NOSUCHCHANNEL(nick, channel) \
    (std::string(":server 403 ") + nick + " " + channel + " :No such channel\r\n")

// 404 - Cannot send to channel
#define ERR_CANNOTSENDTOCHAN(nick, channel) \
    (std::string(":server 404 ") + nick + " " + channel + " :Cannot send to channel\r\n")

// 421 - Unknown command
#define ERR_UNKNOWNCOMMAND(nick, command) \
    (std::string(":server 421 ") + nick + " " + command + " :Unknown command\r\n")

// 431 - No nickname given
#define ERR_NONICKNAMEGIVEN(nick) \
    (std::string(":server 431 ") + nick + " :No nickname given\r\n")

// 432 - Erroneous nickname
#define ERR_ERRONEUSNICKNAME(nick, badnick) \
    (std::string(":server 432 ") + nick + " " + badnick + " :Erroneous nickname\r\n")

// 433 - Nickname already in use
#define ERR_NICKNAMEINUSE(nick, badnick) \
    (std::string(":server 433 ") + nick + " " + badnick + " :Nickname is already in use\r\n")

// 441 - User not in channel
#define ERR_USERNOTINCHANNEL(nick, target, channel) \
    (std::string(":server 441 ") + nick + " " + target + " " + channel + " :They aren't on that channel\r\n")

// 442 - Not on channel
#define ERR_NOTONCHANNEL(nick, channel) \
    (std::string(":server 442 ") + nick + " " + channel + " :You're not on that channel\r\n")

// 443 - User already on channel
#define ERR_USERONCHANNEL(nick, target, channel) \
    (std::string(":server 443 ") + nick + " " + target + " " + channel + " :is already on channel\r\n")

// 461 - Not enough parameters
#define ERR_NEEDMOREPARAMS(nick, command) \
    (std::string(":server 461 ") + nick + " " + command + " :Not enough parameters\r\n")

// 462 - Already registered
#define ERR_ALREADYREGISTRED(nick) \
    (std::string(":server 462 ") + nick + " :You may not reregister\r\n")

// 464 - Password incorrect
#define ERR_PASSWDMISMATCH(nick) \
    (std::string(":server 464 ") + nick + " :Password incorrect\r\n")

// 471 - Channel is full (+l)
#define ERR_CHANNELISFULL(nick, channel) \
    (std::string(":server 471 ") + nick + " " + channel + " :Cannot join channel (+l)\r\n")

// 473 - Invite only channel (+i)
#define ERR_INVITEONLYCHAN(nick, channel) \
    (std::string(":server 473 ") + nick + " " + channel + " :Cannot join channel (+i)\r\n")

// 475 - Bad channel key (+k)
#define ERR_BADCHANNELKEY(nick, channel) \
    (std::string(":server 475 ") + nick + " " + channel + " :Cannot join channel (+k)\r\n")

// 476 - Bad channel mask
#define ERR_BADCHANMASK(channel) \
    (std::string(":server 476 * ") + channel + " :Bad Channel Mask\r\n")

// 482 - Not channel operator
#define ERR_CHANOPRIVSNEEDED(nick, channel) \
    (std::string(":server 482 ") + nick + " " + channel + " :You're not channel operator\r\n")

// 411 ERR_NORECIPIENT (add this after 482)
#define ERR_NORECIPIENT(nick, command) \
    (std::string(":server 411 ") + nick + " :No recipient given (" + command + ")\r\n")

// 412 ERR_NOTEXTTOSEND (add this after 411)
#define ERR_NOTEXTTOSEND(nick) \
    (std::string(":server 412 ") + nick + " :No text to send\r\n")

#endif