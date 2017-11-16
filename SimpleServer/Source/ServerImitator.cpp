#include <iostream>
#include <thread>
#include <conio.h>
#include <string>
#include <algorithm>

#include "ServerImitator.h"


ServerImitator::ServerImitator(const int sendingPort, const int recivingPort, const int backlog)
	: WinsockInterface()
	, _sendingPort(sendingPort)
	, _recivingPort(recivingPort)
	, _backlog(backlog)
	, _readfds()
	, _clientSockets()
{
	_clientSockets.resize(_MAX_CLIENTS);
}

ServerImitator::ServerImitator(ServerImitator&& other) noexcept
	: WinsockInterface()
	, _sendingPort(other._sendingPort)
	, _recivingPort(other._recivingPort)
	, _backlog(other._backlog)
	, _readfds()
	, _clientSockets()
{
	_clientSockets.resize(_MAX_CLIENTS);

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

void ServerImitator::waitLoop()
{
	int addrLen = sizeof(SOCKADDR_IN);

	std::string sbuf;

	SOCKADDR_IN address;

	SOCKET clientSocket;

	std::cout << "\n\n\nWaiting for connections...\n\n";

	while (true)
	{
		memset(_message, 0, _MAXRECV);

		// Clear the socket fd set.
		FD_ZERO(&_readfds);

		// Add master socket to fd set.
		FD_SET(_socketReceive, &_readfds);

		// Add child sockets to fd set.
		for (int i = 0; i < _MAX_CLIENTS; i++)
		{
			clientSocket = _clientSockets[i];
			if (clientSocket > 0)
			{
				FD_SET(clientSocket, &_readfds);
			}
		}

		// Wait for an activity on any of the sockets, timeout is NULL, so wait indefinitely.
		int activity = select(0, &_readfds, nullptr, nullptr, nullptr);

		if (activity == SOCKET_ERROR)
		{
			std::cout << "\nSELECT CALL FAILED WITH ERROR CODE: " << WSAGetLastError() << std::endl;
			system("pause");
			exit(ErrorType::FAILED_SELECT_CALL);
		}

		// If something happened on the master socket, then it's an incoming connection.
		if (FD_ISSET(_socketReceive, &_readfds))
		{ 
			SOCKET newSocket = accept(_socketReceive, reinterpret_cast<SOCKADDR*>(&address),
										static_cast<int*>(&addrLen));
			if (newSocket == SOCKET_ERROR)
			{
				perror("Accept failed.");
				system("pause");
				exit(ErrorType::FAILED_ACCEPT_NEW_CLIENT);
			}

			// Get IP address back and print it.
			inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);

			// Inform user of socket number — used in send and receive commands.
			std::cout << "New connection, socket fd is " << newSocket << ", ip is: " << 
				_message << ", port: " << ntohs(address.sin_port) << '\n';

			std::string tmp = "985000 0 940000 - 180000 0 0 10 0 0 ";
			const char* mes = tmp.c_str();
			int intLength	= static_cast<int>(strlen(mes));

			// Send new connection chacking message.
			if (send(newSocket, mes, intLength, 0) != intLength)
			{
				perror("Send failed.");
			}

			std::cout << "Test message sent successfully.\n\n";

			// Add new socket to array of sockets.
			for (int i = 0; i < _MAX_CLIENTS; i++)
			{
				if (_clientSockets[i] == 0)
				{
					_clientSockets[i] = newSocket;
					std::cout << "Adding to list of sockets at index " << i << '\n';
					break;
				}
			}
		}

		// Else its some IO operation on some other socket. :)
		for (int i = 0; i < _MAX_CLIENTS; i++)
		{
			clientSocket = _clientSockets[i];
			// If client presend in read sockets.            
			if (FD_ISSET(clientSocket, &_readfds))
			{
				// Get details of the client.
				getpeername(clientSocket, reinterpret_cast<SOCKADDR*>(&address),
							static_cast<int*>(&addrLen));

				// Check if it was for closing , and also read the incoming message
				// recv does not place a null terminator at the end of the string 
				// (whilst printf %s assumes there is one).
				int valRead = recv(clientSocket, _buffer, _MAXRECV, 0);

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
						closesocket(clientSocket);
						_clientSockets[i] = 0;
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
					closesocket(clientSocket);
					_clientSockets[i] = 0;
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
						send(clientSocket, toSending.c_str(), toSending.size(), 0);
					}
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
	bindSocket(_socketReceive, _socketReceiveAddress, _recivingPort);

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

	for (const auto& strDatum : strStorage)
	{
		if (!utils::isCorrectNumber(strDatum))
		{
			result.clear();
			break;
		}

		result += strDatum;
	}

	return result;
}
