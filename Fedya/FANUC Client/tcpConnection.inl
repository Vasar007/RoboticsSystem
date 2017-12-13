#ifndef TCP_CONNECTION_INL
#define TCP_CONNECTION_INL

template <typename T>
ConnectionTCP<T>::ConnectionTCP(SOCKET soc, MyQueue<T>* sendQueue, MyQueue<T>* reciveQueue)
    : _soc(soc),
      _sendQueue(sendQueue),
      _receiveQueue(reciveQueue),
      _pocketsReceived("Points from client recived: ",0)
{
}

template <typename T>
void ConnectionTCP<T>::sendCoord()
{
    // Send coordinate.
	if (!_sendQueue->isEmpty())
	{
		std::string message = _sendQueue->front().toString();
		_sendQueue->pop();
		message += "\n";
		send(_soc, message.c_str(), static_cast<int>(message.size()), 0);
	}
}

template <typename T>
int ConnectionTCP<T>::recvCoord()
{
    // Char buffer.
	char recvBuf[256];
	ZeroMemory(recvBuf, 256);
	T rc;

    // Set SOCKET in non-blocking mode.
	struct timeval timeout;
	timeout.tv_sec = 100;
	timeout.tv_usec = 0;
	setsockopt(_soc, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof timeout);

	// Reading data.
    const int l = recv(_soc, recvBuf, 255, 0);
	if (l <= 0 && errno != 0 && errno != EAGAIN && errno != EWOULDBLOCK)
	{
		return -15;
	}
	if (l > 0)
	{
        // Parsing.
		_stringBuffer += recvBuf;
		int tmp = 0;
		for (int i = 0; i < _stringBuffer.size() && tmp < 9; ++i)
		{
			if (_stringBuffer[i] != ' ')
			{
				for (; i < _stringBuffer.size() && ((_stringBuffer[i] >= '0' && _stringBuffer[i] <= '9') || _stringBuffer[i] == '-'); ++i);
				if (i < _stringBuffer.size() && _stringBuffer[i] != ' ')
					return _stringBuffer[i];
				++tmp;
			}
			if (tmp == 9)
			{
				sscanf_s(_stringBuffer.c_str(), "%d %d %d %d %d %d %d %d %d",
					&rc._xr, &rc._yr, &rc._zr, &rc._uw, &rc._up, &rc._uz, &rc._segTime, &rc._typeOfMoving, &rc._control);
				_receiveQueue->push(rc);
                _pocketsReceived.setObject(_pocketsReceived.getObject() + 1);
				_stringBuffer = _stringBuffer.substr(i);
				tmp = 0;
				i = 0;
			}
		}
		return 0;
	}
	return 1;
}

#endif // TCP_CONNECTION_INL