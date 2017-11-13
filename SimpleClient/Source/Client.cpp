#include "Client.h"

#include <iostream>
#include <thread>
#include <string>


Client::Client(const int serverPort, const std::string& serverIP)
: WinsockInterface()
, _serverIP(serverIP)
{
	tryConnect(serverPort, serverIP, _socketSend, _socketSendAddress);
	setTimeout(_socketSend, 100, 0);
}

void Client::setTimeout(const SOCKET& socketForSetting, long seconds, long microseconds) const
{
	TIMEVAL timeout;
	timeout.tv_sec  = seconds;
	timeout.tv_usec = microseconds;
	setsockopt(socketForSetting, SOL_SOCKET, SO_RCVTIMEO,
			   reinterpret_cast<char*>(&timeout), sizeof(timeout));
}

void Client::waitLoop()
{
	int addrlen = sizeof(SOCKADDR_IN);

	SOCKADDR_IN address;

	//RobotData robotData;

	RobotData robotDataDefault;
	robotDataDefault.mValues = { 985000, 0, 940000, -180000 , 0, 0 };
	robotDataDefault.mParameters = { 10, 0, 0 };

	sendData(_socketSend, robotDataDefault.toString());

	robotDataDefault.mValues[0] += 10000;
	robotDataDefault.mValues[1] += 10000;
	robotDataDefault.mValues[2] += 10000;

	
	sendData(_socketSend, robotDataDefault.toString());

	std::cout << "\n\n\nWaiting for reply...\n\n";

	while (true)
	{
		memset(_message, 0, _MAXRECV);

		// Get details of the client.
		getpeername(_socketSend, reinterpret_cast<SOCKADDR*>(&address),
					static_cast<int*>(&addrlen));

		int valRead = recv(_socketSend, _buffer, _MAXRECV, 0);

		if (valRead == SOCKET_ERROR)
		{
			int error_code = WSAGetLastError();
			if (error_code == WSAECONNRESET)
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
				std::cout << "recv failed with error code: " << error_code << std::endl;
			}
		}
		if (valRead == 0)
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
			//send(_socketSend, _buffer, valRead, 0);
		}

		//std::cin >> robotData;

		//std::string temp = "985000 0 940000 180000 0 0 10 0 0 ";

		robotDataDefault.mValues[0] += 10000;
		robotDataDefault.mValues[1] += 10000;
		robotDataDefault.mValues[2] += 10000;

		sendData(_socketSend, robotDataDefault.toString()); //robotData.toString()

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
