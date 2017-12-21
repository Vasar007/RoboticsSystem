#pragma once

#ifndef MY_INTERFACE

#define MY_INTERFACE

#include "Windows.h"
#include <string>
#include <iostream>


static int numberOfCoordSend, numberOfCoordSendAll, numberOfCoordSendToUser, numberOfCoordInSendingQueue,pingTime;
static bool connected = false;

template<typename T>
void showInformation(T object) {
#ifndef WITHOUT_INTERFACE
	std::cout << object << "\n";
#endif //WITHOUT_INTERFACE 
}

inline void updateInformation() {
#ifndef WITHOUT_INTERFACE
	system("cls");
	std::string str;
	if (connected)
		str = "online \n";
	else
		str = "offline\n";
	std::cout << "number of points which we took from user and send to robot: " << numberOfCoordSend
		<< "\nnumber of all pockets which we send to robot: " << numberOfCoordSendAll
		<< "\nnumber of points which we transfirm from robot to user: " << numberOfCoordSendToUser
		<< "\nnumber of points in sending queue: " << numberOfCoordInSendingQueue
		<< "\nping time: " << pingTime
		<< "\nFanuc M-20iA : " << str;///вывод интерфейса
#endif //WITHOUT_INTERFACE
}

inline void updateInformationfast() {
#ifndef WITHOUT_INTERFACE
	static HANDLE hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord;

	coord.X = 60;
	coord.Y = 0;
	SetConsoleCursorPosition(hconsole, coord);
	printf("%d", numberOfCoordSend);

	coord.X = 46;
	coord.Y = 1;
	SetConsoleCursorPosition(hconsole, coord);
	printf("%d", numberOfCoordSendAll);

	coord.X = 56;
	coord.Y = 2;
	SetConsoleCursorPosition(hconsole, coord);
	printf("%d", numberOfCoordSendToUser);

	coord.X = 35;
	coord.Y = 3;
	SetConsoleCursorPosition(hconsole, coord);
	printf("%d    ", numberOfCoordInSendingQueue);

	coord.X = 11;
	coord.Y = 4;
	SetConsoleCursorPosition(hconsole, coord);
	printf("%d    ", pingTime);

	coord.X = 15;
	coord.Y = 5;
	SetConsoleCursorPosition(hconsole, coord);
	if (connected)
		printf("online ");
	else
		printf("offline");

	coord.X = 0;
	coord.Y = 5;
	SetConsoleCursorPosition(hconsole, coord);
#endif //WITHOUT_INTERFACE
}

#endif