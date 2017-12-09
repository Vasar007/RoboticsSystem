#include <iostream>
#include <thread>
#include <conio.h>
#include <string>

#include "ServerImitator.h"


namespace vasily
{

ServerImitator::ServerImitator(const int sendingPort, const int recivingPort, const int backlog)
	: WinsockInterface()
	, _sendingPort(sendingPort)
	, _receivingPort(recivingPort)
	, _backlog(backlog)
	, _clientSocketSend(0)
	, _clientSocketReceive(0)
	, _hasGotCoordSystem(false)
{
}

ServerImitator::ServerImitator(ServerImitator&& other) noexcept
	: WinsockInterface()
	, _sendingPort(other._sendingPort)
	, _receivingPort(other._receivingPort)
	, _backlog(other._backlog)
	, _clientSocketSend(0)
	, _clientSocketReceive(0)
	, _hasGotCoordSystem(other._hasGotCoordSystem ? true : false)
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

void ServerImitator::process(std::atomic_bool& flag)
{
	int addrLen = sizeof(SOCKADDR_IN);

	std::string sbuf;

	SOCKADDR_IN address;

	utils::println("\n\n\nWaiting for connections...\n");

	while (flag)
	{
		memset(_message, 0, _MAXRECV);
		SOCKET socketReceive = accept(_socketReceive, reinterpret_cast<SOCKADDR*>(&address),
										static_cast<int*>(&addrLen));
		if (socketReceive == SOCKET_ERROR)
		{
			perror("Accept failed.");
			std::cin.get();
			exit(static_cast<int>(ErrorType::FAILED_ACCEPT_NEW_CLIENT));
		}

		SOCKET socketSend = accept(_socketSend, reinterpret_cast<SOCKADDR*>(&address),
									static_cast<int*>(&addrLen));
		if (socketSend == SOCKET_ERROR)
		{
			perror("Accept failed.");
			std::cin.get();
			exit(static_cast<int>(ErrorType::FAILED_ACCEPT_NEW_CLIENT));
		}

		_clientSocketReceive	= socketReceive;
		_clientSocketSend		= socketSend;

		_hasGotCoordSystem = false;

		// Get IP address back and print it.
		inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);

		// Inform user of socket number — used in send and receive commands.
		utils::println("New connection, socket FD is", socketReceive, ", ip is:", _message,
						", PORT:", ntohs(address.sin_port));

		if (flag)
		{
			flag = false;
		}

		constexpr std::atomic_int64_t waitingTime = 10LL;
		std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
	}
}

void ServerImitator::waitLoop()
{
	int addrLen = sizeof(SOCKADDR_IN);

	std::string dataBuffer;

	SOCKADDR_IN address;

	utils::println("\n\n\nWaiting for reply...\n");

	std::atomic_bool flag = true;

	waitingForConnections(flag);

	while (true)
	{
		memset(_message, 0, _MAXRECV);

		memset(_buffer, 0, _MAXRECV);

		// Get details of the client.
		getpeername(_clientSocketReceive, reinterpret_cast<SOCKADDR*>(&address),
					static_cast<int*>(&addrLen));

		// Check if it was for closing , and also read the incoming message
		// recv does not place a null terminator at the end of the string 
		// (whilst printf %s assumes there is one).
		const int valRead	= recv(_clientSocketReceive, _buffer, _MAXRECV, 0);
		u_short clientPort	= ntohs(address.sin_port);
		
		// Get IP address back and print it.
		inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);

		if (valRead == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();

			if (errorCode == WSAECONNRESET)
			{
				// Somebody disconnected, get his details and print.
				utils::println("Client disconnected unexpectedly, IP", _message, ", PORT",
								clientPort);
			}
			else
			{
				utils::println("recv failed with error code:", errorCode);
			}
			// Close the socket and mark as 0 for reuse.
			closesocket(_clientSocketReceive);
			_clientSocketReceive = 0;
			closesocket(_clientSocketSend);
			_clientSocketSend = 0;

			waitingForConnections(flag);
		}
		else if (valRead == 0)
		{
			// Somebody disconnected , get his details and print.
			utils::println("Client disconnected , IP", _message, ", PORT", clientPort);

			// Close the socket and mark as 0 for reuse.
			closesocket(_clientSocketReceive);
			_clientSocketReceive = 0;
			closesocket(_clientSocketSend);
			_clientSocketSend = 0;

			waitingForConnections(flag);
		}
		// Echo back the message that came in.
		else if (valRead > 0)
		{
			// Add null character, if you want to use with printf/puts or other string
			// handling functions.
			_buffer[valRead] = '\0';

			if (!_hasGotCoordSystem)
			{
				char coordSystem[2];
				strncpy_s(coordSystem, _buffer, 1);
				coordSystem[1]	= '\0';
				_buffer[0]		= ' ';

				utils::println(coordSystem);
				_hasGotCoordSystem = true;
			}

			utils::println(_message, ':', clientPort, '-', _buffer);

			dataBuffer += _buffer;

			std::string toSending = utils::parseData(dataBuffer);

			if (!toSending.empty())
			{
				sendData(_clientSocketSend, toSending);
				dataBuffer.clear();
			}
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

void ServerImitator::waitingForConnections(std::atomic_bool& flag)
{
	flag = true;

	std::thread processThread(&ServerImitator::process, this, std::ref(flag));
	processThread.join();
}

}
