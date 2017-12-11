#include <iostream>
#include <thread>
#include <conio.h>
#include <string>
#include <cassert>

#include "ServerImitator.h"


namespace vasily
{

ServerImitator::ServerImitator(const int sendingPort, const int recivingPort, const int backlog)
	: WinsockInterface(),
	  _sendingPort(sendingPort),
	  _receivingPort(recivingPort),
	  _backlog(backlog),
	  _clientSocketSend(0),
	  _clientSocketReceive(0),
	  _hasGotCoordSystem(false),
	  _logger(_DEFAULT_IN_FILE_NAME, _DEFAULT_OUT_FILE_NAME)
{
}

ServerImitator::ServerImitator(ServerImitator&& other) noexcept
	: WinsockInterface(),
	  _sendingPort(other._sendingPort),
	  _receivingPort(other._receivingPort),
	  _backlog(other._backlog),
	  _clientSocketSend(0),
	  _clientSocketReceive(0),
	  _hasGotCoordSystem(other._hasGotCoordSystem ? true : false),
	  _logger(_DEFAULT_IN_FILE_NAME, _DEFAULT_OUT_FILE_NAME)
{
	utils::swap(*this, other);
}

ServerImitator& ServerImitator::operator=(ServerImitator&& other) noexcept
{
	if (this != &other)
	{
		utils::swap(*this, other);
	}
	return *this;
}

void ServerImitator::process()
{
	int addrLen = sizeof(SOCKADDR_IN);

	std::string sbuf;

	SOCKADDR_IN address;

	utils::println(std::cout, "\n\n\nWaiting for connections...\n");

	_logger.write("\n\nServer waiting for connections at", utils::getCurrentSystemTime());

	while (!_isRunning)
	{
		memset(_message, 0, _MAXRECV);
		SOCKET socketReceive = accept(_socketReceive, reinterpret_cast<SOCKADDR*>(&address),
									  static_cast<int*>(&addrLen));
		if (socketReceive == SOCKET_ERROR)
		{
			perror("Accept failed.");
			std::cin.get();
			assert(false);
			exit(static_cast<int>(ErrorType::FAILED_ACCEPT_NEW_CLIENT));
		}

		SOCKET socketSend = accept(_socketSend, reinterpret_cast<SOCKADDR*>(&address),
								   static_cast<int*>(&addrLen));
		if (socketSend == SOCKET_ERROR)
		{
			perror("Accept failed.");
			std::cin.get();
			assert(false);
			exit(static_cast<int>(ErrorType::FAILED_ACCEPT_NEW_CLIENT));
		}

		_clientSocketReceive	= socketReceive;
		_clientSocketSend		= socketSend;

		_hasGotCoordSystem		= false;

		// Get IP address back and print it.
		inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);

		// Inform user of socket number — used in send and receive commands.
		utils::println(std::cout, "New connection, socket FD is", socketReceive, ", ip is:", 
					   _message, ", PORT:", ntohs(address.sin_port));

		if (!_isRunning)
		{
			_isRunning = true;
		}

		constexpr std::atomic_int64_t waitingTime = 10LL;
		std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
	}
}

void ServerImitator::waitLoop()
{
	std::string dataBuffer;
	std::string toSending;

	utils::println(std::cout, "\n\n\nWaiting for reply...\n");

	waitingForConnections();

	_logger.write("Server started to receive at", utils::getCurrentSystemTime());

	while (true)
	{
		dataBuffer = receiveData(_clientSocketReceive);

		if (!_isRunning)
		{
			waitingForConnections();
		}

		if (!_hasGotCoordSystem && !dataBuffer.empty())
		{
			const std::string coordSystemStr = dataBuffer.substr(0u, 1u);
			utils::println(std::cout, coordSystemStr);
			_hasGotCoordSystem = true;
		}

		_logger.write(_message, '-', dataBuffer);

		toSending = utils::parseFullData(dataBuffer);

		if (!toSending.empty())
		{
			sendData(_clientSocketSend, toSending);
			dataBuffer.clear();
		}

		constexpr std::atomic_int64_t waitingTime = 10LL;
		std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
	}
}

void ServerImitator::run()
{
	_isRunning = true;
	waitLoop();
}

void ServerImitator::launch()
{
	bindSocket(_socketSend, _socketSendAddress, _sendingPort);
	bindSocket(_socketReceive, _socketReceiveAddress, _receivingPort);

	listenOn(_socketSend, _backlog);
	listenOn(_socketReceive, _backlog);
}

void ServerImitator::waitingForConnections()
{
	// Close the socket and mark as 0 for reuse.
	closesocket(_clientSocketReceive);
	_clientSocketReceive = 0;
	closesocket(_clientSocketSend);
	_clientSocketSend = 0;

	_isRunning = false;

	std::thread processThread(&ServerImitator::process, this);
	processThread.join();
}

}
