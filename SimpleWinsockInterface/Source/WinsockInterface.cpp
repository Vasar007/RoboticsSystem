#include <iostream>
#include <cassert>
#include <thread>
#include <string>

#include "WinsockInterface.h"


WinsockInterface::WinsockInterface()
	: _wsaData()
	, _socketSend()
	, _socketReceive()
	, _socketSendAddress()
	, _socketReceiveAddress()
	, _addressInfo(nullptr)
	, _isRunning(false)
	, _isInitialized(false)
{
	_buffer		= new char[_MAXRECV];
	_message	= new char[_MAXRECV];
}

WinsockInterface::~WinsockInterface() noexcept
{
	// Delete buffers.
	delete[] _buffer;
	delete[] _message;

	if (_isInitialized)
	{
		// The closing of the socket and Winsock.
		closesocket(_socketSend);
		closesocket(_socketReceive);
		WSACleanup();
	}
}

void WinsockInterface::init()
{
	_isInitialized = true;

	initWinsock(_wsaData);
	initSocket(_socketSend);
	initSocket(_socketReceive);
}

void WinsockInterface::initWinsock(WSADATA& wsaData) const
{
	// Initialization Winsock implementation.
	std::cout << "Initializing Winsock...";

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		std::cout << "\n\nFAILED. ERROR CODE: " << WSAGetLastError() << std::endl;
		system("pause");
		exit(ErrorType::FAILED_INITIALIZE_WSDATA);
	}

	std::cout << " done.\n";
}

void WinsockInterface::initSocket(SOCKET& socketToInit, const int aiProtocol) const
{
	// Initialization socket.
	std::cout << "Initialized socket.\n";

	socketToInit = socket(AF_INET, SOCK_STREAM, aiProtocol);

	if (socketToInit == INVALID_SOCKET)
	{
		std::cout << "\nCOULD NOT CREATE SOCKET." << std::endl;
		system("pause");
		exit(ErrorType::FAILED_CREATE_SOCKET);
	}

	std::cout << "Socket created.\n";

}

void WinsockInterface::bindSocket(const SOCKET& socketToBind, SOCKADDR_IN& socketAddress, 
									const int port) const
{
	u_short usPort = static_cast<u_short>(port);

	// Set socket settings.
	socketAddress.sin_family			= AF_INET;
	socketAddress.sin_addr.S_un.S_addr	= INADDR_ANY;
	socketAddress.sin_port				= htons(usPort);

	// Binding to a specific address and port.
	if (bind(socketToBind, reinterpret_cast<SOCKADDR*>(&socketAddress),
		sizeof socketAddress) == SOCKET_ERROR)
	{
		std::cout << "\nBIND FAILED." << std::endl;
		system("pause");
		exit(ErrorType::FAILED_BIND);
	}

	std::cout << "Bind done.\n";
}

void WinsockInterface::listenOn(const SOCKET& socketToList, const int backlog) const
{
	// Check if input data is correct (backlog can has only positive value).
	assert(backlog > 0);

	// Include "listening" mode for receiving incoming connections.
	if (listen(socketToList, backlog) == SOCKET_ERROR)
	{
		std::cout << "\nLISTEN FAILED." << std::endl;
		system("pause");
		exit(ErrorType::FAILED_LISTEN);
	}

	std::cout << "Enabled listening.\n";

	// Cleaning addresses.
	freeaddrinfo(_addressInfo.get());
}

bool WinsockInterface::tryConnect(const int port, const std::string& ip, 
									const SOCKET& socketToConnect, SOCKADDR_IN& socketAddress) const
{
	const char* serverIP	= ip.c_str();
	u_short usPort			= static_cast<u_short>(port);

	// Set socket settings.
	socketAddress.sin_family	= AF_INET;
	socketAddress.sin_port		= htons(usPort);
	inet_pton(AF_INET, serverIP, &socketAddress.sin_addr);

	// The connection to the server.
	if (connect(socketToConnect, reinterpret_cast<SOCKADDR*>(&socketAddress),
		sizeof socketAddress) == SOCKET_ERROR)
	{
		std::cout << "\nCONNECTION TO SERVER WAS FAILED." << std::endl;

		return false;

		//system("pause");
		//exit(ErrorTypes::FAILED_CONNECT);
	}

	std::cout << "Connected successfully.\n\n";
	
	return true;
}

inline bool WinsockInterface::isRun() const
{
	return _isRunning;
}

void WinsockInterface::sendData(const SOCKET& socketToSend, const std::string& data) const
{
	const char* dataChar = data.c_str();

	// Sending data on socket.
	if (send(socketToSend, dataChar, strlen(dataChar), 0) == SOCKET_ERROR)
	{
		std::cout << "SEND FAILED." << std::endl;
	}
	std::cout << "Sent data: " << data << " successfully.\n\n";
}
