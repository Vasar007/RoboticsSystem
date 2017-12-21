#ifndef ROBOT_SEND_INL
#define ROBOT_SEND_INL

#include "MyQueue.hpp"

template <typename T>
int RobotSend<T>::sendCoord(const T rc)
{
    // Transform coordinates to string.
	std::string str = rc.toString();

    // Adding coordinate to cloneQueue.
	_cloneQueue->push(rc);

    // Sending.
	if (send(_sockSend, str.c_str(), static_cast<int>(str.size()), 0) == SOCKET_ERROR)
	{
		closesocket(_sockSend);
		return 1;
	}
	return 0;
}

template <typename T>
RobotSend<T>::RobotSend(SOCKET soc, MyQueue<T>* cloneQueue, int accuracy) 
    : _sockSend(soc),
      _cloneQueue(cloneQueue),
      _accuracy(accuracy)
{
}

template <typename T>
int RobotSend<T>::moveRobot(const T rc,const bool forceSend)
{
    // if we need send.
	if (forceSend == _wasFirstPoint || _previousCoord != rc)
	{
        // If distance is too large.
		if (_previousCoord.difference(rc) > _accuracy || !_wasFirstPoint)
		{
			sendCoord(rc);
		}

        // Set previous coordinate.
		_previousCoord = rc;

        // Sending coordinate.
		sendCoord(rc);
		_wasFirstPoint = true;

		return 0;
	}
	return 1;
}

template <typename T>
int RobotSend<T>::sendPreviousCoord()
{
	if (_wasFirstPoint)
	{
		sendCoord(_previousCoord);
		return 0;
	}
	return 1;
}

template <typename T>
int RobotSend<T>::resendCloneQueue()
{
    // Queue buffer.
	MyQueue<T> sendingQueue;
	
    // Copy clone queue.
    if (!_cloneQueue->isEmpty())
    {
		_cloneQueue->swap(sendingQueue);
    }
	
    // Resending coordinates.
    while (!sendingQueue.isEmpty())
	{
		sendCoord(sendingQueue.front());
		sendingQueue.pop();
	}
	return 0;
}
#endif // ROBOT_SEND_INL
