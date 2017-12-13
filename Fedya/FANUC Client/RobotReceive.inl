#ifndef ROBOT_RECIEVE_INL
#define ROBOT_RECIEVE_INL

#include "MyQueue.hpp"

template <typename T>
int RobotReceieve<T>::receiveCoord()
{
    // Create buffer.
	char recvbuf[128];
	ZeroMemory(recvbuf, 128);
	T robotCoord;

    // Set SOCKET in non-blocking mode.
	struct timeval timeout;
	timeout.tv_sec = _receieveTime;
	timeout.tv_usec = 0;
	setsockopt(_sockRecv, SOL_SOCKET, SO_RCVTIMEO,
        reinterpret_cast<char*>(&timeout), sizeof timeout);
	
    // Try to read coordinates.
	const int iResult = recv(_sockRecv, recvbuf, 127, 0);

    // If nothing come.
	if (iResult <= 0)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			return -15;
		}
	}
	else
	{
        // Pasing.
		_stringBuffer += recvbuf;
		int coordsNumber = 0;
		for (int i = 0; i < _stringBuffer.size(); ++i)
		{
			if (_stringBuffer[i] != ' ')
			{
				while (i < _stringBuffer.size() && _stringBuffer[i] != ' ')
					++i;
				if (i < _stringBuffer.size() && _stringBuffer[i] == ' ')
					++coordsNumber;
			}
			if (coordsNumber == 7)
			{
				sscanf_s(_stringBuffer.c_str(), "%d %d %d %d %d %d %d", &robotCoord._xr,
                    &robotCoord._yr, &robotCoord._zr, &robotCoord._uw, &robotCoord._up,
                    &robotCoord._uz, &robotCoord._segTime);
				robotCoord._typeOfMoving = 0;
				robotCoord._control = 0;
				_returnQueue->push(robotCoord);
				_prevCoord = robotCoord;
				if (!_cloneQueue->isEmpty())
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
RobotReceieve<T>::RobotReceieve(SOCKET socRecv, MyQueue<T>* cloneQueue,
    MyQueue<T>* returnQueue, int readTime)
    : _sockRecv(socRecv),
      _cloneQueue(cloneQueue),
      _returnQueue(returnQueue),
      _receieveTime(readTime)
{
}

#endif // ROBOT_RECIEVE_INL