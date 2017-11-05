#pragma once

#ifndef INITIALISATION

#define INITIALISATION

#include "RobotCoord.hpp"
#include "MyThread.hpp"
#include "MyQueue.hpp"
#include <winsock2.h>
#include "SocketWorking.hpp"
#include "FanucComunication.hpp"
#include <iostream>

//#pragma comment(lib, "ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
//#pragma comment (lib, "AdvApi32.lib")


inline int startWorking(SOCKET& sockSend, SOCKET& sockrecv, const char* serveraddr, int ports, int portr, int syscoord,
	MyThread *threadSend, MyThread *threadRecv, int *segtime, double robotSpeed, MyQueue<RobotCoord> *fanucSendQueue,
	MyQueue<RobotCoord> *fanucReverseQueue, int disconnectTime2, MyQueue<RobotCoord> *cloneQueue) {

	intialise();

	sockSend = socket(AF_INET, SOCK_STREAM, 0);
	///создание сокеа отправки

	sockrecv = socket(AF_INET, SOCK_STREAM, 0);
	///содание сокета приема

	const int iResult = conSocket(sockSend, sockrecv, serveraddr, ports, portr, disconnectTime2);
	if (iResult) {
		std::cout << "Conection error " << iResult << " " << WSAGetLastError() << std::endl;
		closesocket(sockSend);
		closesocket(sockrecv);
		return -4;
	}
	/// подлючение к роботу

	//type of cordiates:0 = JOINT | 1 = JOGFRAME | 2 = WORLDFRAME | 3 = TOOLFRAME | 4 = USER FRAME
	char coord[3] = { 0,0,0 };
	sprintf_s(coord, "%d ", syscoord);
	if (send(sockSend, coord, 1, 0) == SOCKET_ERROR) {
		std::cout << "Conection error " << WSAGetLastError() << std::endl;
		closesocket(sockSend);
		return -5;
	}//отправляем буфер
	 ///выбор системы координат

	threadSend->startThread(sendParalel, segtime, sockSend, fanucSendQueue, cloneQueue);

	threadRecv->startThread(reverseStream, segtime, robotSpeed, sockrecv, sockSend, fanucReverseQueue, cloneQueue);

	return 0;
}

inline void tryConnect(SOCKET& sockSend, SOCKET& sockrecv, const char* serveraddr, int ports, int portr, int syscoord,
	MyThread *threadSend, MyThread *threadRecv, int *segtime, double robotSpeed, MyQueue<RobotCoord> *fanucSendQueue,
	MyQueue<RobotCoord> *fanucReverseQueue, int disconnectTime2, MyQueue<RobotCoord> *cloneQueue) {
	int steps = 0;
	while (startWorking(sockSend, sockrecv, serveraddr, ports, portr, syscoord, threadSend, threadRecv
		, segtime, robotSpeed, fanucSendQueue, fanucReverseQueue, disconnectTime2, cloneQueue) < 0) {
		myInterface::MyShower::getInstance().show("reconecting");
		Sleep(disconnectTime2);
		myInterface::MyShower::getInstance().update();
		++steps;
		if (steps >= 3) {
			healServerSecondConnection(serveraddr, portr, disconnectTime2);
			steps = 0;
		}
	}
}

inline void finishWorking(SOCKET& sockSend, SOCKET& sockRecv, MyThread *threadSend, MyThread *threadRecv) {
	(*threadSend).join();

	(*threadRecv).join();

	closesocket(sockSend);
	closesocket(sockRecv);

	sockSend = INVALID_SOCKET;
	sockRecv = INVALID_SOCKET;

	deintialise();
}

void restart(SOCKET& sockSend, SOCKET& sockrecv, const char* serveraddr, int ports, int portr, int syscoord,
	MyThread *threadSend, MyThread *threadRecv, int *segtime, double robotSpeed, MyQueue<RobotCoord> *fanucSendQueue,
	MyQueue<RobotCoord> *fanucReverseQueue, int disconnectTime2, MyQueue<RobotCoord> *cloneQueue) {

	finishWorking(sockSend, sockrecv, threadSend, threadRecv);

	tryConnect(sockSend, sockrecv, serveraddr, ports, portr, syscoord, threadSend,
		threadRecv, segtime, robotSpeed, fanucSendQueue, fanucReverseQueue, disconnectTime2, cloneQueue);

	myInterface::MyShower::getInstance().update();
}


#endif