//  Пример непростого TCP клиента

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <conio.h>

#include "tcpServer.hpp"
#include "RobotCommunication.hpp"
#include "MyQueue.hpp"


#include"RobotCoord.hpp"

int main()
{
    // Initializing winSock.
	SocketWorking::getInstance().launchWinSock();

    // Queue with coordinates from client to robot.
    MyQueue<RobotCoord> fromOtherProcesToFanuc("from client to robot");
    
    // Queue with coordinates from robot to client.
    MyQueue<RobotCoord> fromFanucToOtherProces("from robot to client");

    // Comunication with fanuc.
	RobotConnect<RobotCoord> fanuc("sets.txt", &fromOtherProcesToFanuc, &fromFanucToOtherProces);
	
	fanuc.startMainLoop();

    // Initialize server.
	ServerTCP<RobotCoord> serv(9997, 500);

	serv.supportOneConnection(&fromFanucToOtherProces, &fromOtherProcesToFanuc);

    //control commands
	while (true) {
		if (_kbhit()) {
			const int c = _getch();
			if (c == '+')
            {
				break;
			}
            if (c == '-')
            {
                fanuc.restartMainLoop();
            }
		}
		Sleep(20);
	}

    serv.stopServer();

	fanuc.stopMainLoop();

	SocketWorking::getInstance().closeWinSock();

	return 0;
}

