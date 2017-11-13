#pragma once

#ifndef ROBOT_SEND

#define ROBOT_SEND
#include "MyQueue.hpp"

template<typename T>
class RobotSend
{
	SOCKET _sockSend;
	MyQueue<T>* _cloneQueue;
	int _acyrancy;

	int sendCoord(T rc)
	{
		std::string str = rc.toString();
		//myInterface::MyShower::getInstance().show(str);

		_cloneQueue->push(rc);
		if (send(_sockSend, str.c_str(), str.size(), 0) == SOCKET_ERROR) 
		{
			closesocket(_sockSend);
			return 1;
		}
		return 0;
	}

public:
	bool _wasFirstPoint;
	T _prevCoord;

	RobotSend(SOCKET soc, MyQueue<T> *cloneQueue, int acyrancy = 1000):
	_sockSend(soc),_cloneQueue(cloneQueue),_acyrancy(acyrancy)
	{
		_wasFirstPoint = false;
	}

	int sendRobotCoord(T rc, bool forceSend = false) {

		if (forceSend == _wasFirstPoint || _prevCoord != rc) {

			if (_prevCoord.difference(rc) > _acyrancy || !_wasFirstPoint) 
			{
				sendCoord(rc);
			}

			_prevCoord = rc;

			sendCoord(rc);
			_wasFirstPoint = true;

#ifdef LOCAL_DEBUG
			std::cout << (std::string)"++++++++++++++:" + (std::string)str;
			putchar('\n');
#endif // LOCAL_DEBUG

			return 0;
		}
		return 1;
	}

	int sendPrevCoord()
	{
		if(_wasFirstPoint)
		{
			sendCoord(_prevCoord);
			return 0;
		}
		return 1;
	}

	int resendCloneQueue()
	{
		MyQueue<T> send;
		if(!_cloneQueue->empty())
			_cloneQueue->swap(send);
		while(!send.empty())
		{
			sendRobotCoord(send.front());
			send.pop();
		}
		return 0;
	}
};

#endif