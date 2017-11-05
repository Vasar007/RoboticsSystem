// main.cpp : Defines the entry point for echo_fedya.
//

#include"roboSender.hpp"
#include<conio.h>



int connectToRobotServer(SOCKET ans, const char* serveraddr, int port1, int disconnectTime2) {
	unsigned long value = 1;
	if (ioctlsocket(ans, FIONBIO, &value) == SOCKET_ERROR)
		return INVALID_SOCKET;

	sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(port1);
	destAddr.sin_addr.s_addr = inet_addr(serveraddr);

	// адрес сервера получен – пытаемся установить соединение 
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
				return ans;

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

int main() {
	int tmp,L;
	RoboSender rs;
	char buf[64];

	RobotCoord rc;
	int stepc,stepu;

#ifdef JOINT
	stepc = stepu = 10000;
#else
	stepu = 15000;
	stepc = 100000;
#endif

	rc._typeOfMoving = 0;
	rc._segTime = 10;
#ifdef JOINT
	/*rc.Xr = 55000;
	rc.Yr = -45000;
	rc.Zr = 0;
	rc.Uw = 0;
	rc.Up = 0;
	rc.Uz = 0;//*/
	rc._xr = 0;
	rc._yr = 0;
	rc._zr = 0;
	rc._uw = 0;
	rc._up = -90000;
	rc._uz = 0;//*/
#else
	rc._xr = 985000;
	rc._yr = 0;
	rc._zr = 940000;
	rc._uw = -180000;
	rc._up = 0;
	rc._uz = 0;
#endif
	while (0==0) {
		ZeroMemory(buf, 64);
		L = recv(rs.getSoc(), buf, 63, 0);
		if(L>0)
			std::cout << buf;

		if (_kbhit()) {
			char c = _getch();
			if (c == 'a') {
				rc._xr -= stepc;
				rc._segTime = 50;
			}
			else if (c == 'q') {
				rc._xr += stepc;
				rc._segTime = 50;
			}
			else if (c == 's') {
				rc._yr -= stepc;
				rc._segTime = 10;
			}
			else if (c == 'w') {
				rc._yr += stepc;
				rc._segTime = 10;
			}
			else if (c == 'd') {
				rc._zr -= stepc;
				rc._segTime = 10;
			}
			else if (c == 'e') {
				rc._zr += stepc;
				rc._segTime = 10;
			}
			else if (c == 'f') {
				rc._uw -= stepu;
				rc._segTime = 10;
			}
			else if (c == 'r') {
				rc._uw += stepu;
				rc._segTime = 10;
			}
			else if (c == 'g') {
				rc._up -= stepu;
				rc._segTime = 10;
			}
			else if (c == 't') {
				rc._up += stepu;
				rc._segTime = 10;
			}
			else if (c == 'h') {
				rc._uz -= stepu;
				rc._segTime = 10;
			}
			else if (c == 'y') {
				rc._uz += stepu;
				rc._segTime = 10;
			}
			else if (c == 'H') {
#ifdef JOINT
				rc._xr = 0;
				rc._yr = 0;
				rc._zr = 0;
				rc._uw = 0;
				rc._up = -90000;
				rc._uz = 0;
				rc._segTime = 300;
#else
				rc._xr = 985000;
				rc._yr = 0;
				rc._zr = 940000;
				rc._uw = -180000;
				rc._up = 0;
				rc._uz = 0;
#endif
			}
			else if(c == '3')
			{
				rc._control = 3;
			}
			else if(c == '4')
			{
				rc._control = 4;
			}
			else if(c == '5')
			{
				rc._control = 5;
			}
			else if(c == '+')
			{
				std::cin >> rc;
			}
		}
		rs.sendCoord(rc);
		Sleep(50);
	}

	//cleaning
}

