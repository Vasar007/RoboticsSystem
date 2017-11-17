#ifndef ROBOT_SEND_DIFINITION
#define ROBOT_SEND_DIFINITION
#pragma once

#include "MyQueue.hpp"

template <typename T>
int RobotSend<T>::sendCoord(T rc)
{
	std::string str = rc.toString();
	_cloneQueue->push(rc);
	if (send(_sockSend, str.c_str(), static_cast<int>(str.size()), 0) == SOCKET_ERROR)
	{
		closesocket(_sockSend);
		return 1;
	}
	return 0;
}

template <typename T>
RobotSend<T>::RobotSend(SOCKET soc, MyQueue<T>* cloneQueue, int acyrancy) :
	_sockSend(soc), _cloneQueue(cloneQueue), _acyrancy(acyrancy)
{
	_wasFirstPoint = false;
}

template <typename T>
int RobotSend<T>::sendRobotCoord(T rc, bool forceSend)
{
	if (forceSend == _wasFirstPoint || _prevCoord != rc)
	{
		if (_prevCoord.difference(rc) > _acyrancy || !_wasFirstPoint)
		{
			sendCoord(rc);
		}

		_prevCoord = rc;

		sendCoord(rc);
		_wasFirstPoint = true;

		return 0;
	}
	return 1;
}

template <typename T>
int RobotSend<T>::sendPrevCoord()
{
	if (_wasFirstPoint)
	{
		sendCoord(_prevCoord);
		return 0;
	}
	return 1;
}

template <typename T>
int RobotSend<T>::resendCloneQueue()
{
	MyQueue<T> send;
	if (!_cloneQueue->empty())
		_cloneQueue->swap(send);
	while (!send.empty())
	{
		sendRobotCoord(send.front());
		send.pop();
	}
	return 0;
}
#endif
