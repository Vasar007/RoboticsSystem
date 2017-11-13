#include "WinsockInterface.h"

#include <iostream>
#include <cassert>
#include <thread>


WinsockInterface::WinsockInterface()
: _masterSocket()
, _wsaData()
{
	_buffer = new char[_MAXRECV];

	_message = new char[_MAXRECV];

	initWinsock();
	initSocket();
}

WinsockInterface::~WinsockInterface()
{
	// The closing of the socket and WinsockAPI.
	closesocket(_masterSocket);		// May be shutdown(_masterSocket, 1);
	WSACleanup();
}

void WinsockInterface::initWinsock()
{
	// Initialization WinsockAPI.
	std::cout << "Initializing Winsock...\n";
	if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != 0)
	{
		std::cout << "\nFAILED. ERROR CODE: " << WSAGetLastError() << std::endl;
		system("pause");
		exit(ErrorType::failedInitializeWsdata);
	}
	std::cout << "Initialized Winsock.\n";
}

void WinsockInterface::initSocket(int aiProtocol)
{
	// Initialization socket.
	std::cout << "Initialized socket.\n";
	if ((_masterSocket = socket(AF_INET, SOCK_STREAM, aiProtocol)) == INVALID_SOCKET)
	{
		std::cout << "\nCOULD NOT CREATE SOCKET." << std::endl;
		system("pause");
		exit(ErrorType::failedCreateSocket);
	}
	std::cout << "Socket created.\n";

}

void WinsockInterface::bindSocket(int port)
{
	// Set socket settings.
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_addr.S_un.S_addr = INADDR_ANY;
	_serverAddress.sin_port = htons(port);

	// Binding to a specific address and port.
	if (bind(_masterSocket, reinterpret_cast<sockaddr*>(&_serverAddress),
		sizeof(_serverAddress)) == SOCKET_ERROR)
	{
		std::cout << "\nBIND FAILED." << std::endl;
		system("pause");
		exit(ErrorType::failedBind);
	}
	std::cout << "Bind done.\n";
}

void WinsockInterface::listenOn(int backlog) const
{
	assert(backlog > 0);

	// Include "listening" mode for receiving incoming connections.
	if (listen(_masterSocket, backlog) == SOCKET_ERROR)
	{
		std::cout << "\nLISTEN FAILED." << std::endl;
		system("pause");
		exit(ErrorType::failedListen);
	}
	std::cout << "Enabled listening.\n\n";
}

bool WinsockInterface::tryConnect(int port, std::string ip)
{
	// Set socket settings again.
	const char* serverIp = ip.c_str();

	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(port);
	inet_pton(AF_INET, serverIp, &_serverAddress.sin_addr);

	// The connection to the server.
	if (connect(_masterSocket, reinterpret_cast<sockaddr*>(&_serverAddress),
		sizeof(_serverAddress)) == SOCKET_ERROR)
	{
		std::cout << "\nCONNECTION TO SERVER WAS FAILED." << std::endl;
		return false;
		//system("pause");
		//exit(ErrorTypes::FAILED_CONNECT);
	}
	std::cout << "Connected successfully.\n\n";
	return true;
}

bool WinsockInterface::isRun() const
{
	return _isRunning;
}

void WinsockInterface::sendData(std::string data) const
{
	const char* dataChar = data.c_str();

	if (send(_masterSocket, dataChar, strlen(dataChar), 0) == SOCKET_ERROR)
	{
		std::cout << "SEND FAILED." << std::endl;
	}
	std::cout << "Sent data: " << data << " successfully.\n\n";
}
