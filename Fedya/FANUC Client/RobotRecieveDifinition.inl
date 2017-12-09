#ifndef ROBOT_RECIEVE_DIFINITION
#define ROBOT_RECIEVE_DIFINITION
#pragma once

#include "MyQueue.hpp"

template <typename T>
int RobotRecieve<T>::readCoord()
{
	char recvbuf[128];
	ZeroMemory(recvbuf, 128);
	T rc;

	struct timeval timeout;
	timeout.tv_sec = _readTime;
	timeout.tv_usec = 0;
	setsockopt(_sockRecv, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof timeout);
	//для unix-систем убрать (char*)

	const int iResult = recv(_sockRecv, recvbuf, 127, 0);

	if (iResult <= 0)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			return -15;
		}
	}
	else
	{
		_stringBuffer += recvbuf;
		int coordsNumber = 0;
		for (int i = 0; i < _stringBuffer.size(); i++)
		{
			if (_stringBuffer[i] != ' ')
			{
				while (i < _stringBuffer.size() && _stringBuffer[i] != ' ')
					i++;
				if (i < _stringBuffer.size() && _stringBuffer[i] == ' ')
					++coordsNumber;
			}
			if (coordsNumber == 7)
			{
				sscanf_s(_stringBuffer.c_str(), "%d %d %d %d %d %d %d", &rc._xr, &rc._yr, &rc._zr, &rc._uw, &rc._up, &rc._uz, &rc._segTime);
				rc._typeOfMoving = 0;
				rc._control = 0;
				_returnQueue->push(rc);
				_prevCoord = rc;
				if (!_cloneQueue->empty())
				{
					_cloneQueue->pop();
				}
				_stringBuffer = _stringBuffer.substr(i);
				i = 0;
				coordsNumber = 0;
			}
		}
	}
	return 0;
}

template <typename T>
RobotRecieve<T>::RobotRecieve(SOCKET socRecv, MyQueue<T>* cloneQueue, MyQueue<T>* returnQueue, int readTime) :
	_sockRecv(socRecv)
	, _cloneQueue(cloneQueue)
	, _returnQueue(returnQueue)
	, _readTime(readTime)
{
	_stringBuffer = "";
}

#endif