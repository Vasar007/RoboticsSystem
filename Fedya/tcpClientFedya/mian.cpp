#include "roboSender.hpp"
#include <conio.h>


int main() {
	RoboSender rs;
	char buf[64];

	RobotCoord rc;

	//if JOINT
	//stepc = stepu = 10000;

	int stepu = 15000;
	int stepc = 100000;

	rc._typeOfMoving = 0;
	rc._segTime = 10;

	//if JOINT
	/*rc.Xr = 55000;
	rc.Yr = -45000;
	rc.Zr = 0;
	rc.Uw = 0;
	rc.Up = 0;
	rc.Uz = 0;
	rc._xr = 0;
	rc._yr = 0;
	rc._zr = 0;
	rc._uw = 0;
	rc._up = -90000;
	rc._uz = 0;*/

	rc._xr = 985000;
	rc._yr = 0;
	rc._zr = 940000;
	rc._uw = -180000;
	rc._up = 0;
	rc._uz = 0;

	while (true) 
	{
		ZeroMemory(buf, 64);
		int L = recv(rs.getSoc(), buf, 63, 0);
		if (L > 0)
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

				//	if JOINT
				//rc._xr = 0;
				//rc._yr = 0;
				//rc._zr = 0;
				//rc._uw = 0;
				//rc._up = -90000;
				//rc._uz = 0;
				//rc._segTime = 300;

				rc._xr = 985000;
				rc._yr = 0;
				rc._zr = 940000;
				rc._uw = -180000;
				rc._up = 0;
				rc._uz = 0;

			}
			else if (c == '3')
			{
				rc._control = 3;
			}
			else if (c == '4')
			{
				rc._control = 4;
			}
			else if (c == '5')
			{
				rc._control = 5;
			}
			else if (c == '+')
			{
				std::cin >> rc;
			}
		}
		rs.sendCoord(rc);
		Sleep(50);
	}

	//cleaning
}

