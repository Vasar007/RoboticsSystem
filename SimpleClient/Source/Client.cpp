#include <iostream>
#include <thread>

#include "Client.h"


namespace vasily
{

Client::Client(const int serverPort, const std::string_view serverIP)
	: WinsockInterface()
	, _robotData()
	, _serverIP(serverIP)
	, _serverPort(serverPort)
{
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

	utils::println("\n\n\nThread started...\n");

	while (true)
	{
		memset(_message, 0, _MAXRECV);

		memset(_buffer, 0, _MAXRECV);

		// Get details of the client.
		getpeername(_socketSend, reinterpret_cast<SOCKADDR*>(&address),
					static_cast<int*>(&addrlen));

		int valRead			= recv(_socketSend, _buffer, _MAXRECV, 0);
		u_short serverPort	= ntohs(address.sin_port);

		// Get IP address back and print it.
		inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);
		
		if (valRead == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();

			if (errorCode == WSAECONNRESET)
			{
				// Somebody disconnected, get his details and print.
				utils::println("Server disconnected unexpectedly, IP", _message, ", PORT",
								serverPort);
			}
			else
			{
				utils::println("recv failed with error code:", errorCode);
			}

			_isRunning = false;
			tryReconnect(serverPort);
		}
		else if (valRead == 0)
		{
			// Somebody disconnected, get his details and print.
			utils::println("Server disconnected, IP", _message, ", PORT", serverPort);
		}
		// Echo back the message that came in.
		else if (valRead > 0)
		{
			// Add null character, if you want to use with printf/puts or other string 
			// handling functions.
			_buffer[valRead] = '\0';

			utils::println(_message, ':', serverPort, '-', _buffer);
		}
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

	utils::println("\n\n\nWaiting for reply...\n");

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

void Client::setServerIP(std::string_view newServerIP)
{
	_serverIP = newServerIP;
}

void Client::tryReconnect(const int port)
{
	while (!_isRunning)
	{
		closesocket(_socketSend);
		closesocket(_socketReceive);

		initSocket(_socketSend);
		initSocket(_socketReceive);
		//_isRunning = tryConnect(port, _serverIP, _socketSend, _socketSendAddress);
		_isRunning = tryConnect(9999, _serverIP, _socketSend, _socketSendAddress);
		_isRunning = tryConnect(9998, _serverIP, _socketReceive, _socketReceiveAddress);

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
	//tryConnect(_serverPort, _serverIP, _socketSend, _socketSendAddress);
	tryConnect(9999, _serverIP, _socketSend, _socketSendAddress);
	tryConnect(9998, _serverIP, _socketReceive, _socketReceiveAddress);
	//setTimeout(_socketSend, 1000, 0);
}

}
