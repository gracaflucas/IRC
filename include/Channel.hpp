#pragma once
#include "Client.hpp"

class Client; 
class Server; 

class Channel
{
	private:
	std::string				_name;
	std::string				_topic[3];
	std::string				_key;
	std::string 			_mode;
	std::vector<int>		_channelClients;
	std::vector<int>		_channelAdmins;
	std::vector<int>		_channelInvites;
	bool					_inviteOnly;
	bool					_topicRestricted;
	int						_userLimit;

	public:
		Channel(const std::string &name);
		~Channel();

		//GETTERS
		std::string	getName() const;
		std::string	getKey() const;
		std::string	getTopic(unsigned int index) const;
		std::string	getMode() const;
		std::vector<int> &getClients();
		std::vector<int> &getAdmins();
		std::vector<int> &getChannelInvites();
		bool			getInviteOnly() const;
		bool			getTopicRestricted() const;
		int				getUserLimit() const;

	
		//SETTERS
		void	setName(const std::string &newName);
		void	setTopic(const std::string &newTopic, std::string client, std::string time);
		void	setKey(const std::string &newKey);
		void	setMode(const std::string &newMode);
		void	setInviteOnly(bool status);
		void	setTopicRestricted(bool status);
		void	setUserLimit(int limit);

		void	addClient(Client *client);
		void	addAdmin(Client *client);
		void	removeAdmin(int socket);
		void	removeClient(int socket);
		void	addChannelInvite(const Client *client);
};
