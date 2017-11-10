#include "Client.h"

#include <iostream>
#include <thread>


Client::Client(int port, std::string serverIP)
: WinsockInterface()
, _serverIP(serverIP)
{
	tryConnect(port);
	sendData("2580");
}

bool Client::tryConnect(int port)
{
	// Set socket settings.
	const char* serverIP = _serverIP.c_str();

	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(port);
	inet_pton(AF_INET, serverIP, &_serverAddress.sin_addr);

	// The connection to the server.
	if (connect(_masterSocket, reinterpret_cast<sockaddr*>(&_serverAddress),
		sizeof(_serverAddress)) == SOCKET_ERROR)
	{
		std::cout << "\nConnection to server was failed." << std::endl;
		return false;
		//system("pause");
		//exit(ErrorTypes::FAILED_CONNECT);
	}
	std::cout << "Connected successfully.\n" << std::endl;
	return true;
}

void Client::waitLoop()
{
	int addrlen = sizeof(struct sockaddr_in);

	char* message = new char[_MAXRECV];

	sockaddr_in address;

	std::cout << "\n\n\nWaiting for reply...\n\n";

	while (true)
	{
		memset(message, 0, _MAXRECV);

		// Get details of the client.
		getpeername(_masterSocket, reinterpret_cast<sockaddr*>(&address),
			static_cast<int*>(&addrlen));

		int valRead = recv(_masterSocket, _buffer, _MAXRECV, 0);

		if (valRead == SOCKET_ERROR)
		{
			int error_code = WSAGetLastError();
			if (error_code == WSAECONNRESET)
			{
				// Get IP address back and print it.
				inet_ntop(AF_INET, &address.sin_addr, message, INET_ADDRSTRLEN);
				// Somebody disconnected, get his details and print.
				std::cout << "Server disconnected unexpectedly, ip " << message << " , port " <<
					ntohs(address.sin_port) << '\n';

				_isRunning = false;
				tryReconnect(ntohs(address.sin_port));
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
			// Somebody disconnected, get his details and print.
			std::cout << "Server disconnected, ip " << message << " , port " <<
				ntohs(address.sin_port) << '\n';
		}

		// Echo back the message that came in.
		else
		{
			// Add null character, if you want to use with printf/puts or other string 
			// handling functions.
			_buffer[valRead] = '\0';
			// Get IP address back and print it.
			inet_ntop(AF_INET, &address.sin_addr, message, INET_ADDRSTRLEN);
			std::cout << message << ":" << ntohs(address.sin_port) << " - " << _buffer << '\n';
			send(_masterSocket, _buffer, valRead, 0);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

std::string Client::getServerIP() const
{
	return _serverIP;
}

void Client::setServerIP(std::string newServerIP)
{
	_serverIP = newServerIP;
}

void Client::tryReconnect(int port)
{
	while (!_isRunning)
	{
		closesocket(_masterSocket);

		initSocket();
		_isRunning = tryConnect(port);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void Client::run()
{
	_isRunning = true;
	waitLoop();
}
