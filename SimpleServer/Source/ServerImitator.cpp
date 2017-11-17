#include <iostream>
#include <thread>
#include <conio.h>
#include <string>
#include <algorithm>

#include "ServerImitator.h"


ServerImitator::ServerImitator(const int sendingPort, const int recivingPort, const int backlog)
	: WinsockInterface()
	, _sendingPort(sendingPort)
	, _receivingPort(recivingPort)
	, _backlog(backlog)
	, _clientSocketSend(0)
	, _clientSocketReceive(0)
{
}

ServerImitator::ServerImitator(ServerImitator&& other) noexcept
	: WinsockInterface()
	, _sendingPort(other._sendingPort)
	, _receivingPort(other._receivingPort)
	, _backlog(other._backlog)
	, _clientSocketSend(0)
	, _clientSocketReceive(0)
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

void ServerImitator::sendReply(std::string& data) const
{
	int addrLen = sizeof(SOCKADDR_IN);

	SOCKADDR_IN address;

	std::cout << "\n\n\nThread started...\n\n";
	
	// Get details of the client.
	getpeername(_clientSocketSend, reinterpret_cast<SOCKADDR*>(&address),
				static_cast<int*>(&addrLen));
	
	///std::string toSending = "985000 0 940000 180000 0 0 10 ";
	
	if (!data.empty())
	{
		sendData(_clientSocketSend, data);
	}
}

void ServerImitator::receive()
{
	int addrLen = sizeof(SOCKADDR_IN);

	std::string sbuf;

	SOCKADDR_IN address;

	std::cout << "\n\n\nWaiting for connections...\n\n";

	while (true)
	{
		memset(_message, 0, _MAXRECV);

		// Get details of the client.
		getpeername(_clientSocketReceive, reinterpret_cast<SOCKADDR*>(&address),
					static_cast<int*>(&addrLen));

		// Check if it was for closing , and also read the incoming message
		// recv does not place a null terminator at the end of the string 
		// (whilst printf %s assumes there is one).
		int valRead = recv(_clientSocketReceive, _buffer, _MAXRECV, 0);

		if (valRead == SOCKET_ERROR)
		{
			int error_code = WSAGetLastError();
			if (error_code == WSAECONNRESET)
			{
				// Get IP address back and print it.
				inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);
				// Somebody disconnected, get his details and print.
				std::cout << "Client disconnected unexpectedly, ip " << _message <<
					" , port " << ntohs(address.sin_port) << '\n';

				// Close the socket and mark as 0 in list for reuse.
				closesocket(_clientSocketReceive);
				_clientSocketReceive = 0;
			}
			else
			{
				std::cout << "recv failed with error code: " << error_code << '\n';
			}
		}
		else if (valRead == 0)
		{
			// Get IP address back and print it.
			inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);
			// Somebody disconnected , get his details and print.
			std::cout << "Client disconnected , ip " << _message << " , port " <<
				ntohs(address.sin_port) << '\n';

			// Close the socket and mark as 0 in list for reuse.
			closesocket(_clientSocketReceive);
			_clientSocketReceive = 0;
		}
		// Echo back the message that came in.
		else if (valRead > 0)
		{
			// Add null character, if you want to use with printf/puts or other string
			// handling functions.
			_buffer[valRead] = '\0';
			// Get IP address back and print it.
			inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);
			std::cout << _message << ':' << ntohs(address.sin_port) << " - " <<
				_buffer << '\n';

			sbuf += _buffer;

			std::string toSending = parseData(sbuf);

			///toSending = "985000 0 940000 180000 0 0 10 ";

			if (!toSending.empty())
			{
				sendData(_clientSocketSend, toSending);
			}

			//std::thread sendThread(&ServerImitator::sendReply, this, toSending);
			//sendThread.detach();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void ServerImitator::waitLoop()
{
	int addrLen = sizeof(SOCKADDR_IN);

	std::string sbuf;

	SOCKADDR_IN address;

	std::cout << "\n\n\nWaiting for reply...\n\n";

	bool flag = true;

	while (true)
	{
		memset(_message, 0, _MAXRECV);
		SOCKET socketReceive = accept(_socketReceive, reinterpret_cast<SOCKADDR*>(&address),
										static_cast<int*>(&addrLen));
		if (socketReceive == SOCKET_ERROR)
		{
			perror("Accept failed.");
			system("pause");
			exit(ErrorType::FAILED_ACCEPT_NEW_CLIENT);
		}

		SOCKET socketSend = accept(_socketSend, reinterpret_cast<SOCKADDR*>(&address),
			static_cast<int*>(&addrLen));
		if (socketSend == SOCKET_ERROR)
		{
			perror("Accept failed.");
			system("pause");
			exit(ErrorType::FAILED_ACCEPT_NEW_CLIENT);
		}
		
		_clientSocketReceive = socketReceive;
		_clientSocketSend = socketSend;


		// Get IP address back and print it.
		inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);

		// Inform user of socket number — used in send and receive commands.
		std::cout << "New connection, socket fd is " << socketReceive << ", ip is: " <<
			_message << ", port: " << ntohs(address.sin_port) << '\n';

		if (flag)
		{
			std::thread reciveThread(&ServerImitator::receive, this);
			reciveThread.detach();
			flag = false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		break;
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

std::string ServerImitator::parseData(const std::string& data) const
{
	std::string result;
	std::vector<std::string> strStorage;
	utils::split(data, strStorage);

	strStorage.erase(std::remove(strStorage.begin(), strStorage.end(), ""), strStorage.end());

	const std::size_t NUMBER_OF_COORDINATES_IN_ONE_STRUCTURE = 9u;

	if (strStorage.size() % NUMBER_OF_COORDINATES_IN_ONE_STRUCTURE != 0u)
	{
		return { "" };
	}

	int count = 0;

	for (const auto& strDatum : strStorage)
	{
		if (!utils::isCorrectNumber(strDatum))
		{
			result.clear();
			break;
		}

		if (count % 9 != 8 && count % 9 != 7)
		{
			result += strDatum + " ";
		}

		++count;
	}

	return result.substr(1);
}
