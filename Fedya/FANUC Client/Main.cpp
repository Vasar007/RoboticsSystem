// ѕример непростого TCP клиента

#define WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#define WITH_COSTUL_SHOULD_REMOVE

#define WITHOUT_POINTS_SAVER
//#define WITHOUT_INTERFACE

#include <string>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include "MyShower.hpp"
#include "RobotComunication.hpp"
#include "MyQueue.hpp"
#include "tcpServer.hpp"

int main(int argc, char* argv[])
{
	SocketWorking::getInstance().initialise();

	MyQueue<RobotCoord> fromOtherProcesToFanuc("RAM reciving queue"), fromFanucToOtherProces("RAM sending queue");

	//usingRAM::SharingMemory<RobotCoord> buf(L"skdfhgsdkjvisdu\0", L"qwertyuiop\0", &fromOtherProcesToFanuc, &fromFanucToOtherProces, 128);
	//создаем буфер дл€ передачи через опертивною пам€ть

	RobotConnect<RobotCoord> fanuc("sets.txt", &fromOtherProcesToFanuc, &fromFanucToOtherProces);
	//кдасс общени€ с роботом фанук
	
	fanuc.startMainLoop();

	ServerTCP<RobotCoord> serv(7070,1000);

	//serv.forceAccept(&fromFanucToOtherProces, &fromOtherProcesToFanuc, 1000);
	serv.supportOneConnection(&fromFanucToOtherProces, &fromOtherProcesToFanuc);

	//updateInformation();
	myInterface::MyShower::getInstance().update();

	int iResult = 0;
	while (true) {
		if (_kbhit()) {
			const char c = _getch();
			if (c == '+') {
				std::string comand;
				std::getline(std::cin, comand);
				if (comand == "exit") {
					break;
				}
				if (comand == "status") {
					//updateInformation();
					myInterface::MyShower::getInstance().update();
				}
				else if (comand == "restart") {
					fanuc.restartMainLoop();
					//updateInformation();
					myInterface::MyShower::getInstance().update();
				}
				else {
					//updateInformation();
					myInterface::MyShower::getInstance().update();
				}
				iResult = 0;
			}
		}

		//-------------------------------------------------------------------------------------------
		Sleep(5);

		++iResult;
		if (iResult >= 20) {
			//updateInformation();
			myInterface::MyShower::getInstance().updateQiuck();
			iResult = 0;
		}
	}//закрытие большого while'a
	 //-------------------------------------------------------------------------

	fanuc.stopMainLoop();
	//*/

	SocketWorking::getInstance().deintialise();

	return 0;
}

