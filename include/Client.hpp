/* 1 . Construtor e estado básico

Implementar o Client(const int socket, std::string &hostname):

Guardar _socket e _hostname.

Inicializar _isAuth = false, _insertPassword = false.

_nick, _user, _realName ficam vazios.

_buffer vazio.

_channels vazio.

2. getters e setters
3. buffer de mensagem
4.auth
5.comunicacao
6. canais*/

#pragma once
#ifndef CLIENT_HPP
# define CLIENT_HPP
#include <string>
#include <unistd.h>   // close()
#include <sys/socket.h> // send()
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <string.h>
#include <poll.h>
#include <vector> //*
#include <cstdlib>
#include <map>
#include <algorithm>
#include "Channel.hpp"
#include <fcntl.h>
#include <stdlib.h>
#include <limits>
#include <netinet/in.h>
#include <arpa/inet.h>

class Channel;

class Client
	{
	private:
		int _socket; //identificador do cliente
		bool _isAuth;//se o cliente já passou por autenticação
		bool _insertPass;
		std::string _hostName; //ip/hostname do cliente
		std::string _buffer;//dados recebidos que ainda não foram processados.
		std::string _nick;
		std::string _user;
		std::string _realName;
		std::vector<Channel *> _channels;

	public:
		Client(int socket, const std::string &hostname);
		~Client();

		 //GETTERS    
		bool insertedPass() const;
		int  getSocket() const;
		std::string getUser() const;
		std::string getNick() const;
		std::string getRealName() const;
		std::string getHostname() const;
		std::vector<Channel *>& getChannels();
		bool isAuth() const;
		const std::string getPrefix() const;
		std::string& getBuffer();

		//SETTERS	
		void setAuth(bool status);
		void setInsertPass(bool status);
		void setNick(const std::string &nick);
		void setUser(const std::string &user);
		void setRealName(const std::string &realName);

		// Buffer
		void appendBuffer(const std::string &str);
		void clearBuffer();
		
		// Comunicação
		void	sendErrorMessage(const std::string &msg);
		void	sendMsgToClient(Client *client, const std::string &msg);
		// create channel
};

#endif