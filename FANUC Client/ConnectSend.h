#pragma once

#ifndef ConnectSend_tmp

#define ConnectSend_tmp

#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")



inline int conSocket1(SOCKET soc, const char* serveraddr, int port, int disconnectTime2) {

	unsigned long value = 1;
	if (ioctlsocket(soc, FIONBIO, &value) == SOCKET_ERROR)
		return SOCKET_ERROR;

	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	dest_addr.sin_addr.s_addr = inet_addr(serveraddr);

	// адрес сервера получен – пытаемся установить соединение 
	if (connect(soc, (sockaddr *)&dest_addr, sizeof(dest_addr)) == SOCKET_ERROR) {
		//std::cout << "Connect error " << WSAGetLastError() << std::endl;
		//system("pause");
		int result = WSAGetLastError();
		if (result == WSAEWOULDBLOCK) {
			fd_set Write, Err;
			TIMEVAL Timeout;
			int TimeoutSec = 2*disconnectTime2/1000; // timeout after 10 seconds

			FD_ZERO(&Write);
			FD_ZERO(&Err);
			FD_SET(soc, &Write);
			FD_SET(soc, &Err);


			Timeout.tv_sec = TimeoutSec;
			Timeout.tv_usec = 0;

			int iResult = select(0,         //ignored
					NULL,      //read
					& Write,    //Write Check
					& Err,      //Error Check
					& Timeout);
			if (iResult == 0) {
				WSASetLastError(result);
				return 1;
			}
			else
			{
				if (FD_ISSET(soc, &Write))
					return 0;

				if (FD_ISSET(soc, &Err)) {
					WSASetLastError(result);
					return 2;
				}
			}
		}
		else {
			WSASetLastError(result);
			return 3;
		}
	}
	return 0;
}



#endif //ConnectSend_tmp