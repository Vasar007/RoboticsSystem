#pragma once

#ifndef ROBOT_RECIEVE

#define ROBOT_RECIEVE
#include <fstream>

std::ofstream out("logs2.txt");

template<typename T>
class RobotRecieve
{
	SOCKET _sockRecv;
	MyQueue<T>* _cloneQueue;
	MyQueue<T>* _returnQueue;
	std::string _sbuf;
	int _readTime;
public:

	T _prevCoord;

	//Функция приема:
	int readCoord()
	{
		char recvbuf[128];
		ZeroMemory(recvbuf, 128);
		T rc;

		struct timeval timeout;
		timeout.tv_sec = _readTime;
		timeout.tv_usec = 0;
		setsockopt(_sockRecv, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof timeout); //для unix-систем убрать (char*)

		const int iResult = recv(_sockRecv, recvbuf, 127, 0);

		if (iResult <= 0)  // Даня, ты этот кусок не менял, я заметил!!! Это же я его писал :D
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				return -15;
			}
		}
		else
		{

			_sbuf += recvbuf;
#ifdef WITH_COSTUL_SHOULD_REMOVE

			if((std::string(recvbuf)).size() == 38)
			{
				sscanf_s(_sbuf.c_str(), "%d %d %d %d %d %d %d", &rc._xr, &rc._yr, &rc._zr, &rc._uw, &rc._up, &rc._uz, &rc._segTime);
				_sbuf = "";
				rc._typeOfMoving = 0;
				rc._control = 0;
				if (!_cloneQueue->empty()) {
					_cloneQueue->pop();
				}
				_returnQueue->push(rc);
			}
#endif
			int coordsNumber = 0;
			for (int i = 0;i < _sbuf.size();i++) 
			{
				if (_sbuf[i] != ' ') {

					while (i < _sbuf.size() && _sbuf[i] != ' ')
						i++;
					if (i < _sbuf.size() && _sbuf[i] == ' ')
						++coordsNumber;
				}
				if (coordsNumber == 7) 
				{
					sscanf_s(_sbuf.c_str(), "%d %d %d %d %d %d %d", &rc._xr, &rc._yr, &rc._zr, &rc._uw, &rc._up, &rc._uz, &rc._segTime);
					rc._typeOfMoving = 0;
					rc._control = 0;
					_returnQueue->push(rc);
					_prevCoord = rc;
					if (!_cloneQueue->empty()) 
					{
						_cloneQueue->pop();
					}
					_sbuf = _sbuf.substr(i);
					i = 0;
					coordsNumber = 0;
				}
			}
			out << _sbuf << "\n1 "<<fflush;
			/*
			bool f = false;
			for(int i=0;i<_sbuf.size();i++)
			{
				if(_sbuf[i]!=' ')
				{
					f = true;
					break;
				}
			}
			if(f)
				out << "from fanuc not complit pocket|" << _sbuf << "\n"<<fflush;//*/

#ifdef LOCAL_DEBUG
			std::cout << "--------------:";
			for (int i = 0;i < iResult;i++)
				putchar(recvbuf[i]);
			putchar('\n');
#endif // LOCAL_DEBUG

		}
		return 0;
	}

	RobotRecieve(SOCKET socRecv, MyQueue<T> *cloneQueue, MyQueue<T> *returnQueue, int readTime)
	: _sockRecv(socRecv)
	, _cloneQueue(cloneQueue)
	, _returnQueue(returnQueue)
	, _readTime(readTime)
	{
		_sbuf = "";
	}
};

#endif