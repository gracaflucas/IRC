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

class Channel;

class Client
	{
	private:
		int _socket; //identificador do cliente
		std::string _hostname; //ip/hostname do cliente
		std::string _buffer;//dados recebidos que ainda não foram processados.
		bool _isAuth;//se o cliente já passou por autenticação

	public:
		Client(int socket, const std::string &hostname);
		~Client();

		// Getters
		int getSocket() const;
		const std::string &getHostname() const;
		std::string &getBuffer();
		bool isAuth() const;

		// Setters
		void setAuth(bool status);

		// Buffer
		void appendBuffer(const std::string &str);
		void clearBuffer();

		// Comunicação
		void sendErrorMessage(const std::string &msg);
		void	sendMsgToClient(Client *client, const std::string &msg);
};

#endif