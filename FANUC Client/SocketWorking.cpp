#include<Ws2tcpip.h>
#include <winsock2.h>

#include "SocketWorking.hpp"
#include "MyShower.hpp"

SocketWorking::SocketWorking()
{
	_isInitialized = false;
}

SocketWorking& SocketWorking::getInstance()
{
	static SocketWorking instance;
	return instance;
}

int SocketWorking::launchWinSock()
{
	if (!_isInitialized)
	{
		char buff[sizeof(WSADATA)];
		if (WSAStartup(0x202, reinterpret_cast<WSADATA*>(buff)))
		{
			myInterface::MyShower::getInstance().addLog("WSAStart error: ", WSAGetLastError());
			return 1;
		}
		_isInitialized = true;
		return 0;
	}
	return 2;
}


int SocketWorking::closeWinSock()
{
	if (_isInitialized)
	{
		WSACleanup();
		_isInitialized = false;
		return 0;
	}
	return 1;
}


SOCKET SocketWorking::getFreeSocket()
{
	if (!_isInitialized)
	{
		launchWinSock();
	}
	const SOCKET ans = socket(AF_INET, SOCK_STREAM, 0);
	return ans;
}


void SocketWorking::deleteSocket(SOCKET& soc)
{
	if (!_isInitialized)
	{
		launchWinSock();
	}
	closesocket(soc);
	soc = INVALID_SOCKET;
}



SOCKET SocketWorking::getConnectedSocket(const char* serveraddr, int port, int disconnectTime2)
{
    // Initializing.
	if (!_isInitialized)
	{
		launchWinSock();
	}
	
    // New SOCKET.
    const SOCKET ans = getFreeSocket();
	
    // Type of blocking SOCKET.
    unsigned long value = 1;
	
    // Set SOCKET to non-blocking mode.
    if (ioctlsocket(ans, FIONBIO, &value) == SOCKET_ERROR)
		return INVALID_SOCKET;

    // Getting address of server.
	sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(static_cast<u_short>(port));
	inet_pton(AF_INET, serveraddr, &destAddr.sin_addr);

	// Launching connection.
	if (connect(ans, reinterpret_cast<sockaddr *>(&destAddr), sizeof destAddr) == SOCKET_ERROR)
	{
		// Get previous error.
		const int result = WSAGetLastError();
		
        // If server don't wait connection.
        if (result == WSAEWOULDBLOCK)
		{
            // Collections of SOCKETS.
			fd_set write, err;

            // Struct for setting time.
			TIMEVAL timeout;

			// Filling sets.
            FD_ZERO(&write);
			FD_ZERO(&err);
			FD_SET(ans, &write);
			FD_SET(ans, &err);


            // Timeout in seconds.
			timeout.tv_sec = disconnectTime2 / 1000;
			
            // Timeout in miliseconds.
            timeout.tv_usec = disconnectTime2 % 1000;

            // Checking sets of SOCKETs.
			const int iResult = select(0, // ignored
				nullptr, // read
				&write, // Write Check
				&err, // Error Check
				&timeout);

            // If no SOCKETS find.
			if (iResult == 0)
			{
				WSASetLastError(result);
				return INVALID_SOCKET;
			}

            // If find writable SOCKET.
			if (FD_ISSET(ans, &write))
			{
				return ans;
			}

            // If find error SOCKET.
			if (FD_ISSET(ans, &err))
			{
				WSASetLastError(result);
				return INVALID_SOCKET;
			}
		}
		else
		{
            // If another error -> return it.
			WSASetLastError(result);
			return INVALID_SOCKET;
		}
	}

    // If we connect successfully.
	return ans;
}