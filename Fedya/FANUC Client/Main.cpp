// Пример непростого TCP клиента

#define WIN32_LEAN_AND_MEAN


#define WITHOUT_POINTS_SAVER
//#define WITHOUT_INTERFACE

#include <string>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include "MyShower.hpp"
#include "tcpServer.hpp"
#include "RobotComunication.hpp"
#include "MyQueue.hpp"


#include"RobotCoord.hpp"

int main()
{
	SocketWorking::getInstance().initialise();

	MyQueue<RobotCoord> fromOtherProcesToFanuc("from client to robot"), fromFanucToOtherProces("from robot to client");


	RobotConnect<RobotCoord> fanuc("sets.txt", &fromOtherProcesToFanuc, &fromFanucToOtherProces);
	//кдасс общения с роботом фанук
	
	fanuc.startMainLoop();

	ServerTCP<RobotCoord> serv(9997, 1000);

	serv.supportOneConnection(&fromFanucToOtherProces, &fromOtherProcesToFanuc);

	while (true) {
		if (_kbhit()) {
			const int c = _getch();
			if (c == '+') {
				std::string comand;
				std::getline(std::cin, comand);
				if (comand == "exit") {
					break;
				} if (comand == "restart") {
					fanuc.restartMainLoop();
				}
			}
		}
		Sleep(5);
	}

	fanuc.stopMainLoop();

	SocketWorking::getInstance().deintialise();

	return 0;
}

