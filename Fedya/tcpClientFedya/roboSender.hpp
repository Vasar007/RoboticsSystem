#pragma once
#include <iostream>
#include <Ws2tcpip.h>
#include <WinSock2.h>
#include <thread>
#include <mutex>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


struct RobotCoord
{
	int _xr, _yr, _zr, _uw, _up, _uz, _segTime, _typeOfMoving, _control;
	bool operator != (const RobotCoord& rc) const
	{
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

	RobotCoord()
	{
		_xr = _yr = _zr = _uw = _up = _uz = _segTime = _typeOfMoving = _control = 0;
	}

	std::string toString() const
	{
		char locBuf[128];
		std::fill(locBuf, locBuf + 128, 0);
		sprintf_s(locBuf, "%d %d %d %d %d %d %d %d %d ", _xr, _yr, _zr, _uw,
				  _up, _uz, _segTime, _typeOfMoving, _control);
		return static_cast<std::string>(locBuf);
	}
};//структура пакета предачи

inline std::istream& operator >> (std::istream& in, RobotCoord& rc)
{
	in >> rc._xr >> rc._yr >> rc._zr >> rc._uw >> rc._up >> rc._uz >>
		rc._segTime >> rc._typeOfMoving >> rc._control;
	return in;
}

class RoboSender
{
	SOCKET _soc;
	bool _valid;
public:
	explicit RoboSender(int port = 7070)
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
		{
			std::cout << "WSAStartup error: " << WSAGetLastError() << '\n';
			_soc = INVALID_SOCKET;
			_valid = false;
		}
		else
		{
			_soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			std::string serverIp = "127.0.0.1";

			sockaddr_in destAddr;
			destAddr.sin_family = AF_INET;
			destAddr.sin_port = htons(port);
			inet_pton(AF_INET, serverIp.c_str(), &destAddr.sin_addr);

			if (connect(_soc, reinterpret_cast<sockaddr *>(&destAddr), sizeof destAddr) ==
				SOCKET_ERROR)
			{
				std::cout << "connect error: " << WSAGetLastError() << '\n';
				closesocket(_soc);
				_soc = INVALID_SOCKET;
				_valid = false;
			}
			else
			{

				timeval timeout;
				timeout.tv_sec = 100;
				timeout.tv_usec = 0;
				setsockopt(_soc, SOL_SOCKET, SO_RCVTIMEO, 
						   reinterpret_cast<char*>(&timeout), sizeof(timeout));
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
		if (_valid)
			return _soc;
		return INVALID_SOCKET;
	}//вернуть сокет

	void sendCoord(const RobotCoord& rc)
	{
		if (_valid)
		{
			std::string str = rc.toString();
			if (send(_soc, str.c_str(), str.size(), 0) == SOCKET_ERROR)
			{
				_valid = false;
				closesocket(_soc);
				WSACleanup();
			}
		}
	}//отпраление кординат

	~RoboSender()
	{
		if (_valid)
		{
			closesocket(_soc);
			WSACleanup();
		}
	}
};//класс передачи