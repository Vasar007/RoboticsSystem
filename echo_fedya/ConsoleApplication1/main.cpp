#include "stdafx.h"
#include <iostream>
#include <Ws2tcpip.h>
#include <WinSock2.h>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <queue>
#include <conio.h>
#include <map>
#include <fstream>



bool flag = true;

void go(const SOCKET& stSend, const SOCKET& stGet)
{
	char buf[256];
	std::string sbuf;
	bool was = false;

	timeval timeout;
	timeout.tv_sec = 100;
	timeout.tv_usec = 0;
	setsockopt(stGet, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
	while (true)
	{
		if (_kbhit())
		{
			char c = _getch();
			if (c == '+')
				break;
			if (c == '-')
			{
				flag = false;
				break;
			}
		}

		setsockopt(stGet, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout),
			sizeof(timeout));
		ZeroMemory(buf, 256);
		int l = recv(stGet, buf, 255, 0);
		sbuf += buf;

		if (!was)
		{
			was = true;
			sbuf = sbuf.substr(1);
		}

		if (l > 0)
		{
			int tmp = 0;
			std::string str = "";
			for (int i = 0; i < sbuf.size() && tmp < 9; i++)
			{
				if (sbuf[i] != ' ')
				{
					if (tmp != 7 && tmp != 8)
						str += " ";
					for (i; i < sbuf.size() && ((sbuf[i] >= '0' && sbuf[i] <= '9') 
						|| sbuf[i] == '-'); i++)
					{
						if (tmp != 7 && tmp != 8)
							str += sbuf[i];
					}
					tmp++;
				}
				if (tmp == 9)
				{
					std::cout << clock() << " " << sbuf.substr(0, i) << '\n';
					sbuf = sbuf.substr(i);
					str += " ";
					send(stSend, str.c_str(), str.size(), 0);
					tmp = 0;
					i = 0;
					str.clear();
				}
			}
		}

		Sleep(50);
	}
}

struct addrinfo *serverAddr;

int initialise(SOCKET& soc, int port1)
{
	soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (soc == INVALID_SOCKET)
	{
		//freeaddrinfo(server_addr);
		return WSAGetLastError();
	}
	//creating socket

	sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(port1);
	destAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(soc, reinterpret_cast<sockaddr*>(&destAddr), sizeof(destAddr)) < 0)
	{
		closesocket(soc);
		soc = INVALID_SOCKET;
		std::cout << WSAGetLastError();
		return WSAGetLastError();
	}
	//binding socket

	if (listen(soc, SOMAXCONN) < 0)
	{
		closesocket(soc);
		soc = INVALID_SOCKET;
		return WSAGetLastError();
	}
	//let socket be connectable

	freeaddrinfo(serverAddr);
	//cleaning addres
	return 0;
}

int main() {
	int tmp;
	SOCKET soc1 = INVALID_SOCKET;
	SOCKET soc2 = INVALID_SOCKET;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		std::cout << "WSAStartup error: " << WSAGetLastError();
		system("pause");
		return 1;
	}

	if (((tmp = initialise(soc1, 7072)) != 0) || soc1 == INVALID_SOCKET)
	{
		std::cout << "intialisation error: " << tmp << '\n';
		WSACleanup();
		system("pause");
		return 2;
	}

	if (((tmp = initialise(soc2, 7071)) != 0) || soc2 == INVALID_SOCKET)
	{
		std::cout << "intialisation error: " << tmp << '\n';
		WSACleanup();
		system("pause");
		return 3;
	}

	sockaddr_in destAddr;
	int tvp = sizeof(destAddr);

	//std::thread th;

	while (flag)
	{
		SOCKET st1 = INVALID_SOCKET;
		SOCKET st2 = INVALID_SOCKET;
		st1 = accept(soc1, reinterpret_cast<sockaddr *>(&destAddr), &tvp);
		st2 = accept(soc2, reinterpret_cast<sockaddr *>(&destAddr), &tvp);

		std::cout << "+1\n";

		go(st2, st1);
	}

	closesocket(soc1);
	closesocket(soc2);
	WSACleanup();
	//cleaning

	return 0;
}
