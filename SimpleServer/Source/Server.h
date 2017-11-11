#ifndef SERVER_H
#define SERVER_H
#pragma once

#include "WinsockInterface.h"


class Server : public WinsockInterface
{
	private:		
		const int		 _backlog = 10;

		const int		 _maxClients = 32;
	
		std::string		 _serverIP;

		bool			_isRunning;

		/**
		 * \brief Set of socket descriptors.
		 */
		fd_set			 _readfds;

		std::vector<int> _clientSockets;


		void waitLoop() override;


	public:
		explicit Server(int port, int backlog = 10);

		void run() override;
};

#endif // SERVER_H
