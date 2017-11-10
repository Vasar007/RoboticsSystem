#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<Ws2tcpip.h>
#include<WinSock2.h>
#include<thread>
#include<mutex>
#include<string>

#define SleepTime 50
//#define JOINT

#pragma comment( lib, "wsock32.lib" )

struct RobotCoord {
	int _xr, _yr, _zr, _uw, _up, _uz, _segTime, _typeOfMoving, _control;
	bool operator != (RobotCoord rc) {
		return rc._control != _control ||
			rc._segTime != _segTime ||
			rc._typeOfMoving != _typeOfMoving ||
			rc._up != _up ||
			rc._uw != _uw ||
			rc._uz != _uz ||
			rc._xr != _xr ||
			rc._yr != _yr ||
			rc._zr != _zr;
	}
	RobotCoord() {
		_xr = _yr = _zr = _uw = _up = _uz = _segTime = _typeOfMoving = _control = 0;
	}
	std::string toString() const
	{
		char locBuf[128];
		std::fill(locBuf, locBuf + 128, 0);
		sprintf_s(locBuf, "%d %d %d %d %d %d %d %d %d ", _xr, _yr, _zr, _uw, _up, _uz, _segTime, _typeOfMoving, _control);
		return static_cast<std::string>(locBuf);
	}
};//��������� ������ �������

inline std::istream& operator >> (std::istream& in, RobotCoord& rc)
{
	in >> rc._xr >> rc._yr >> rc._zr >> rc._uw >> rc._up >> rc._uz >> rc._segTime >> rc._typeOfMoving >> rc._control;
	return in;
}

class RoboSender
{
	SOCKET _soc;
	bool _valid;
public:
	explicit RoboSender(int port = 7070)
	{
		WSADATA WsaData;
		if (WSAStartup(MAKEWORD(2, 2), &WsaData) != NO_ERROR)
		{
			std::cout << "WSAStartup error: " << WSAGetLastError()<<"\n";
			_soc = INVALID_SOCKET;
			_valid = false;
		}
		else 
		{
			_soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			sockaddr_in destAddr;
			destAddr.sin_family = AF_INET;
			destAddr.sin_port = htons(port);
			destAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
			
			if (connect(_soc, reinterpret_cast<sockaddr *>(&destAddr), sizeof destAddr) == SOCKET_ERROR)
			{
				std::cout << "connect error: " << WSAGetLastError() << "\n";
				closesocket(_soc);
				_soc = INVALID_SOCKET;
				_valid = false;
			}
			else {

				struct timeval timeout;
				timeout.tv_sec = 100;
				timeout.tv_usec = 0;
				setsockopt(_soc, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
				_valid = true;
			}
		}
	}
	bool isValid()const 
	{
		return _valid;
	}
	SOCKET getSoc() const
	{
		if(_valid)
			return _soc;
		return INVALID_SOCKET;
	}//������� �����
	void sendCoord(RobotCoord rc)
	{
		if (_valid) {
			std::string str = rc.toString();
			if(send(_soc, str.c_str(), str.size(), 0) == SOCKET_ERROR)
			{
				_valid = false;
				closesocket(_soc);
				WSACleanup();
			}
		}
	}//���������� ��������
	~RoboSender()
	{
		if (_valid) {
			closesocket(_soc);
			WSACleanup();
		}
	}
};//����� ��������