#pragma once

#ifndef SOCKET_WORKING
//#define LOCAL_DEBUG
#define SOCKET_WORKING

#include "RobotCoord.hpp"
#include<Ws2tcpip.h>
#include<WinSock2.h>
#include <iostream>

#pragma comment( lib, "wsock32.lib" )

class SocketWorking
{
	bool _isInitialised;

	SocketWorking()
	{	
		_isInitialised = false;
	}

	SocketWorking(const SocketWorking&)
	{
		_isInitialised = false;
	}

	SocketWorking& operator = (SocketWorking&)
	{
		_isInitialised = false;
		return getInstance();
	}
public:
	static SocketWorking& getInstance()
	{
		static SocketWorking instance;
		return instance;
	}

	int initialise() 
	{
		if (!_isInitialised)
		{
			char buff[sizeof(WSADATA)];
			if (WSAStartup(0x202, reinterpret_cast<WSADATA *>(buff))) //вызываем включение библиотеки работы с сокетами, инициализаци€ WinsockAPI
			{
				std::cout << "WSAStart error %d" << WSAGetLastError() << std::endl;
				return 1;
			}
			_isInitialised = true;
			return 0;
		}
		return 2;
	}


	int deintialise() 
	{
		if (_isInitialised) 
		{
			WSACleanup();
			_isInitialised = false;
			return 0;
		}
		return 1;
	}

	SOCKET getFreeSocket()
	{
		if (!_isInitialised)
			initialise();
		const SOCKET ans = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		return ans;
	}

	int deleteSocket(SOCKET& soc)
	{
		if (!_isInitialised)
			initialise();
		closesocket(soc);
		soc = INVALID_SOCKET;
		return 0;
	}

	//‘ункци€ подключени€ к сокету
	SOCKET connectToRobotServer(const char* serveraddr, int port, int disconnectTime2) {
		if (!_isInitialised)
			initialise();
		const SOCKET ans = getFreeSocket();

		unsigned long value = 1;
		if (ioctlsocket(ans, FIONBIO, &value) == SOCKET_ERROR)
			return INVALID_SOCKET;

		sockaddr_in destAddr;
		destAddr.sin_family = AF_INET;
		destAddr.sin_port = htons(port);
		//destAddr.sin_addr.s_addr = inet_addr(serveraddr);
		inet_pton(AF_INET, serveraddr, &destAddr.sin_addr);

		// адрес сервера получен Ц пытаемс€ установить соединение 
		if (connect(ans, reinterpret_cast<sockaddr *>(&destAddr), sizeof destAddr) == SOCKET_ERROR) {
			//std::cout << "Connect error " << WSAGetLastError() << std::endl;
			//system("pause");
			const int result = WSAGetLastError();
			if (result == WSAEWOULDBLOCK) {
				fd_set write, err;
				TIMEVAL timeout;
				const int timeoutSec = 2 * disconnectTime2 / 1000; // timeout after 10 seconds

				FD_ZERO(&write);
				FD_ZERO(&err);
				FD_SET(ans, &write);
				FD_SET(ans, &err);


				timeout.tv_sec = timeoutSec;
				timeout.tv_usec = 0;

				const int iResult = select(0,         //ignored
					nullptr,      //read
					&write,    //Write Check
					&err,      //Error Check
					&timeout);
				if (iResult == 0) {
					WSASetLastError(result);
					return INVALID_SOCKET;
				}
				if (FD_ISSET(ans, &write))
				{
					char* message = new char[1024];
					memset(message, 0, 1024);

					// Get IP address back and print it.
					inet_ntop(AF_INET, &destAddr.sin_addr, message, INET_ADDRSTRLEN);

					// Inform user of socket number Ч used in send and receive commands.
					std::cout << "New connection, socket fd is " << ans << ", ip is: " <<
						message << ", port: " << ntohs(destAddr.sin_port) << '\n';

					volatile auto mes = message;
					volatile auto por = ntohs(destAddr.sin_port);

					return ans;
				}

				if (FD_ISSET(ans, &err)) {
					WSASetLastError(result);
					return INVALID_SOCKET;
				}
			}
			else {
				WSASetLastError(result);
				return INVALID_SOCKET;
			}
		}

		return ans;
	}

};

#endif