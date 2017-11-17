#include "SocketWorking.hpp"

#include<Ws2tcpip.h>
#include <winsock2.h>
#include<iostream>

#include "MyShower.hpp"

SocketWorking::SocketWorking()
{
	_isInitialised = false;
}

inline SocketWorking::SocketWorking(const SocketWorking&)
{
	_isInitialised = false;
}

inline SocketWorking& SocketWorking::operator=(SocketWorking&)
{
	_isInitialised = false;
	return getInstance();
}

SocketWorking& SocketWorking::getInstance()
{
	static SocketWorking instance;
	return instance;
}

int SocketWorking::initialise()
{
	if (!_isInitialised)
	{
		char buff[sizeof(WSADATA)];
		if (WSAStartup(0x202, reinterpret_cast<WSADATA *>(buff)))
			//вызываем включение библиотеки работы с сокетами, инициализация WinsockAPI
		{
			myInterface::MyShower::getInstance().addLog("WSAStart error: ", WSAGetLastError());
			//std::cout << "WSAStart error %d" << WSAGetLastError() << std::endl;
			return 1;
		}
		_isInitialised = true;
		return 0;
	}
	return 2;
}


int SocketWorking::deintialise()
{
	if (_isInitialised)
	{
		WSACleanup();
		_isInitialised = false;
		return 0;
	}
	return 1;
}


SOCKET SocketWorking::getFreeSocket()
{
	if (!_isInitialised)
		initialise();
	const SOCKET ans = socket(AF_INET, SOCK_STREAM, 0);
	return ans;
}


void SocketWorking::deleteSocket(SOCKET& soc)
{
	if (!_isInitialised)
		initialise();
	closesocket(soc);
	soc = INVALID_SOCKET;
}



SOCKET SocketWorking::connectToRobotServer(const char* serveraddr, int port, int disconnectTime2)
{
	if (!_isInitialised)
		initialise();
	const SOCKET ans = getFreeSocket();
	unsigned long value = 1;
	if (ioctlsocket(ans, FIONBIO, &value) == SOCKET_ERROR)
		return INVALID_SOCKET;

	sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(static_cast<u_short>(port));
	inet_pton(AF_INET, serveraddr, &destAddr.sin_addr);

	// адрес сервера получен – пытаемся установить соединение 
	if (connect(ans, reinterpret_cast<sockaddr *>(&destAddr), sizeof destAddr) == SOCKET_ERROR)
	{
		//std::cout << "Connect error " << WSAGetLastError() << std::endl;
		//system("pause");
		const int result = WSAGetLastError();
		if (result == WSAEWOULDBLOCK)
		{
			fd_set write, err;
			TIMEVAL timeout;
			const int timeoutSec = 2 * disconnectTime2 / 1000; // timeout after 10 seconds

			FD_ZERO(&write);
			FD_ZERO(&err);
			FD_SET(ans, &write);
			FD_SET(ans, &err);


			timeout.tv_sec = timeoutSec;
			timeout.tv_usec = 0;

			const int iResult = select(0, //ignored
				nullptr, //read
				&write, //Write Check
				&err, //Error Check
				&timeout);
			if (iResult == 0)
			{
				WSASetLastError(result);
				return INVALID_SOCKET;
			}
			if (FD_ISSET(ans, &write))
				return ans;

			if (FD_ISSET(ans, &err))
			{
				WSASetLastError(result);
				return INVALID_SOCKET;
			}
		}
		else
		{
			WSASetLastError(result);
			return INVALID_SOCKET;
		}
	}
	return ans;
}