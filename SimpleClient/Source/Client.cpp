#include <iostream>
#include <thread>
#include <string>

#include "Client.h"


Client::Client(const int serverPort, const std::string& serverIP)
	: WinsockInterface()
	, _robotData()
	,_serverIP(serverIP)
	,_serverPort(serverPort)
{
}

void Client::setTimeout(const SOCKET& socketForSetting, const long seconds,
						const long microseconds) const
{
	TIMEVAL timeout;
	timeout.tv_sec	= seconds;
	timeout.tv_usec	= microseconds;
	setsockopt(socketForSetting, SOL_SOCKET, SO_RCVTIMEO,
				reinterpret_cast<char*>(&timeout), sizeof timeout);
}

Client::Client(Client&& other) noexcept
	: WinsockInterface()
	, _robotData()
	, _serverIP(other._serverIP)
	, _serverPort(other._serverPort)
{
	utils::swap(*this, other);
}

Client& Client::operator=(Client&& other) noexcept
{
	if (this != &other)
	{
		utils::swap(*this, other);
	}
	return *this;
}

void Client::receive()
{
	int addrlen = sizeof(SOCKADDR_IN);

	SOCKADDR_IN address;

	std::cout << "\n\n\nThread started...\n\n";

	while (true)
	{
		memset(_message, 0, _MAXRECV);

		memset(_buffer, 0, _MAXRECV);

		// Get details of the client.
		getpeername(_socketSend, reinterpret_cast<SOCKADDR*>(&address),
					static_cast<int*>(&addrlen));

		int valRead = recv(_socketSend, _buffer, _MAXRECV, 0);

		if (valRead == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAECONNRESET)
			{
				// Get IP address back and print it.
				inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);

				// Somebody disconnected, get his details and print.
				std::cout << "Server disconnected unexpectedly, ip " << _message << " , port " <<
					ntohs(address.sin_port) << '\n';

				_isRunning = false;
				tryReconnect(ntohs(address.sin_port));
			}
			else
			{
				std::cout << "recv failed with error code: " << errorCode << std::endl;
			}
		}
		else if (valRead == 0)
		{
			// Get IP address back and print it.
			inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);

			// Somebody disconnected, get his details and print.
			std::cout << "Server disconnected, ip " << _message << " , port " <<
				ntohs(address.sin_port) << '\n';
		}
		// Echo back the message that came in.
		else if (valRead > 0)
		{
			// Add null character, if you want to use with printf/puts or other string 
			// handling functions.
			_buffer[valRead] = '\0';

			// Get IP address back and print it.
			inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);
			std::cout << _message << ":" << ntohs(address.sin_port) << " - " << _buffer << '\n';

			std::cout << _buffer << '\n';
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	}
}

void Client::waitLoop()
{
	int addrlen = sizeof(SOCKADDR_IN);

	SOCKADDR_IN address;

	///RobotData robotData;

	// Create default data for robot.
	RobotData robotDataDefault;
	robotDataDefault.mValues		= { 985000, 0, 940000, -180000 , 0, 0 };
	robotDataDefault.mParameters	= { 10, 0, 0 };

	sendData(_socketSend, robotDataDefault.toString());

	robotDataDefault.mValues[0] += 10000;
	robotDataDefault.mValues[1] += 10000;
	robotDataDefault.mValues[2] += 10000;

	
	sendData(_socketSend, robotDataDefault.toString());

	std::thread reciveThread(&Client::receive, this);
	reciveThread.detach();

	std::cout << "\n\n\nWaiting for reply...\n\n";

	while (true)
	{
		memset(_message, 0, _MAXRECV);

		// Get details of the client.
		getpeername(_socketSend, reinterpret_cast<SOCKADDR*>(&address),
					static_cast<int*>(&addrlen));

		///std::cin >> robotData;

		///std::string temp = "985000 0 940000 180000 0 0 10 0 0 ";

		// Changes point to recive data.
		robotDataDefault.mValues[0] += 10000;
		robotDataDefault.mValues[1] += 10000;
		robotDataDefault.mValues[2] += 10000;

		sendData(_socketSend, robotDataDefault.toString()); ///robotData.toString()

		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	}
}

std::string Client::getServerIP() const
{
	return _serverIP;
}

void Client::setServerIP(const std::string& newServerIP)
{
	_serverIP = newServerIP;
}

void Client::tryReconnect(const int port)
{
	while (!_isRunning)
	{
		closesocket(_socketSend);

		initSocket(_socketSend);
		_isRunning = tryConnect(port, _serverIP, _socketSend, _socketSendAddress);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void Client::run()
{
	_isRunning = true;
	waitLoop();
}

void Client::launch()
{
	tryConnect(_serverPort, _serverIP, _socketSend, _socketSendAddress);
	setTimeout(_socketSend, 1000, 0);
}
