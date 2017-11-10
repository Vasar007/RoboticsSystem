#include "Server.h"

#include <iostream>
#include <cassert>
#include <thread>


Server::Server(int port, int backlog)
: WinsockInterface()
, _serverIP()
, _clientSockets()
{
	_clientSockets.resize(_MAX_CLIENTS);

	bindSocket(port);
	listenOn(backlog);
}

void Server::waitLoop()
{
	int addrlen = sizeof(struct sockaddr_in);

	char* message = new char[_MAXRECV];

	sockaddr_in address;

	SOCKET socket;
	SOCKET new_socket;

	std::cout << "\n\n\nWaiting for connections...\n\n";

	while (true)
	{
		memset(message, 0, _MAXRECV);

		// Clear the socket fd set.
		FD_ZERO(&_readfds);

		// Add master socket to fd set.
		FD_SET(_masterSocket, &_readfds);

		// Add child sockets to fd set.
		for (int i = 0; i < _MAX_CLIENTS; i++)
		{
			socket= _clientSockets[i];
			if (socket> 0)
			{
				FD_SET(socket, &_readfds);
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
		if (FD_ISSET(_masterSocket, &_readfds))
		{
			if ((new_socket = accept(_masterSocket, reinterpret_cast<sockaddr*>(&address),
				static_cast<int*>(&addrlen))) == SOCKET_ERROR)
			{
				perror("accept");
				system("pause");
				exit(ErrorType::FAILED_ACCEPT_NEW_CLIENT);
			}

			// Get IP address back and print it.
			inet_ntop(AF_INET, &address.sin_addr, message, INET_ADDRSTRLEN);

			// Inform user of socket number — used in send and receive commands.
			std::cout << "New connection, socket fd is " << new_socket << ", ip is: " << 
				message << ", port: " << ntohs(address.sin_port) << '\n';

			std::string tmp = "ECHO";
			const char* mes = tmp.c_str();
			// Send new connection greeting message.
			if (send(new_socket, mes, strlen(mes), 0) != strlen(mes))
			{
				perror("send failed");
			}

			std::cout << "Welcome message sent successfully.\n\n";

			// Add new socket to array of sockets.
			for (int i = 0; i < _MAX_CLIENTS; i++)
			{
				if (_clientSockets[i] == 0)
				{
					_clientSockets[i] = new_socket;
					std::cout << "Adding to list of sockets at index " << i << '\n';
					break;
				}
			}
		}

		// Else its some IO operation on some other socket. :)
		for (int i = 0; i < _MAX_CLIENTS; i++)
		{
			socket = _clientSockets[i];
			// If client presend in read sockets.            
			if (FD_ISSET(socket, &_readfds))
			{
				// Get details of the client.
				getpeername(socket, reinterpret_cast<sockaddr*>(&address),
					static_cast<int*>(&addrlen));

				// Check if it was for closing , and also read the incoming message
				// recv does not place a null terminator at the end of the string 
				// (whilst printf %s assumes there is one).
				int valRead = recv(socket, _buffer, _MAXRECV, 0);

				if (valRead == SOCKET_ERROR)
				{
					int error_code = WSAGetLastError();
					if (error_code == WSAECONNRESET)
					{
						// Get IP address back and print it.
						inet_ntop(AF_INET, &address.sin_addr, message, INET_ADDRSTRLEN);
						// Somebody disconnected, get his details and print.
						std::cout << "Client disconnected unexpectedly, ip " << message <<
							" , port " << ntohs(address.sin_port) << '\n';

						// Close the socket and mark as 0 in list for reuse.
						closesocket(socket);
						_clientSockets[i] = 0;
					}
					else
					{
						std::cout << "recv failed with error code: " << error_code << std::endl;
					}
				}
				if (valRead == 0)
				{
					// Get IP address back and print it.
					inet_ntop(AF_INET, &address.sin_addr, message, INET_ADDRSTRLEN);
					// Somebody disconnected , get his details and print.
					std::cout << "Client disconnected , ip " << message << " , port " <<
						ntohs(address.sin_port) << '\n';

					// Close the socket and mark as 0 in list for reuse.
					closesocket(socket);
					_clientSockets[i] = 0;
				}

				// Echo back the message that came in.
				else
				{
					// Add null character, if you want to use with printf/puts or other string
					// handling functions.
					_buffer[valRead] = '\0';
					// Get IP address back and print it.
					inet_ntop(AF_INET, &address.sin_addr, message, INET_ADDRSTRLEN);
					std::cout << message << ":" << ntohs(address.sin_port) << " - " << _buffer << 
						'\n';
					send(socket, _buffer, valRead, 0);
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Server::run()
{
	_isRunning = true;
	waitLoop();
}
