#ifndef CLIENT_H
#define CLIENT_H
#pragma once

#include "WinsockInterface.h"


class Client : public WinsockInterface
{
	private:
		std::string _serverIP;


	public:


	private:
		bool tryConnect(int port);
		void tryReconnect(int port);

		void waitLoop() override;

		std::string getServerIP() const;


	public:
		explicit Client(int port, std::string serverIP);

		void setServerIP(std::string newServerIP);
		void run() override;
};

#endif // CLIENT_H
