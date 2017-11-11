#ifndef TCP_SERVER
#define TCP_SERVER
#pragma once

#include <WinSock2.h>
#include "MyThread.hpp"
#include "MyQueue.hpp"
#include "SocketWorking.hpp"
#include "tcpConnection.hpp"
#include<Ws2tcpip.h>

#pragma comment( lib, "ws2_32.lib " )


struct addrinfo* serverAddr;

template<typename T>
class ServerTcp
{
	SOCKET _main;

	ConnectionTCP<T> *_contcp;
	MyThread _conSocketWorkin, _conSocketWorkout;
	MyThread _oneConnection;
	MyThread _paralelAccept;

	MyQueue<SOCKET> _socketConnectionsQueue;

	int _prevRecieve;
	int _timeOut;

	bool _connected = false;

	int initialise(int port)
	{
		_main = SocketWorking::getInstance().getFreeSocket();
		if (_main == INVALID_SOCKET) 
		{
			//freeaddrinfo(server_addr);
			return WSAGetLastError();
		}
		//creating socket

		sockaddr_in destAddr;
		destAddr.sin_family = AF_INET;
		destAddr.sin_port = htons(port);
		destAddr.sin_addr.s_addr = INADDR_ANY;

		if (bind(_main, reinterpret_cast<sockaddr *>(&destAddr), sizeof(destAddr)) < 0) 
		{
			closesocket(_main);
			_main = INVALID_SOCKET;
			std::cout << WSAGetLastError();
			return WSAGetLastError();
		}
		//binding socket

		if (listen(_main, SOMAXCONN) < 0) 
		{
			closesocket(_main);
			_main = INVALID_SOCKET;
			return WSAGetLastError();
		}
		//let socket be connetable

		freeaddrinfo(serverAddr);
		//cleaning addres
		return 0;
	}

	static void tcpWorkin(std::mutex *mt, bool *f, ServerTcp *ins)
	{
		while (true)
		{
			mt->lock();
			if (!*f)
			{
				mt->unlock();
				break;
			}
			mt->unlock();

			ins->_contcp->sendCoord();

			Sleep(10);
		}
	}
	static void tcpWorkout(std::mutex *mt, bool *f, ServerTcp *ins)
	{
		ins->_prevRecieve = clock();
		while (true)
		{
			mt->lock();
			if (!*f)
			{
				mt->unlock();
				break;
			}
			mt->unlock();

			if (ins->_contcp->recvCoord() == 0)
				ins->_prevRecieve = clock();

			if (clock() - ins->_prevRecieve > ins->_timeOut)
				ins->_connected = false;

			Sleep(10);
		}
	}

	static void oneConnection(std::mutex *mt, bool *f, ServerTcp* instance, MyQueue<T> *sendQueue, MyQueue<T> *recieveQueue)
	{
		while (true)
		{
			mt->lock();
			if (!*f)
			{
				mt->unlock();
				break;
			}
			mt->unlock();
			if (!instance->_socketConnectionsQueue.empty())
			{
				instance->closeServer();
				instance->forceAccept(sendQueue, recieveQueue);
				instance->_connected = true;
			}
			Sleep(10);
		}
	}

	static void paralelAccept(std::mutex *mt, bool *f, ServerTcp* instance)
	{
		unsigned long value = 1;
		ioctlsocket(instance->_main, FIONBIO, &value);

		fd_set sSet = { 1,{ instance->_main } };
		timeval timeout = { instance->_timeOut / 500, 0 };
		while (true) 
		{
			mt->lock();
			if (!*f)
			{
				mt->unlock();
				break;
			}
			mt->unlock();
			sSet.fd_count = 1;
			const int selectRes = select(0, &sSet, nullptr, nullptr, &timeout);
			if (selectRes == SOCKET_ERROR) 
			{
				int t = GetLastError();
				return;
			}
			if (selectRes)
			{
				SOCKADDR_IN nsa;
				int sizeofNsa = sizeof(nsa);

				const SOCKET connectedSocket = accept(instance->_main, reinterpret_cast<SOCKADDR *>(&nsa), &sizeofNsa);
				if (connectedSocket == INVALID_SOCKET)
					continue;
				instance->_socketConnectionsQueue.push(connectedSocket);
			}
			Sleep(10);
		}
	}

public:

	explicit ServerTcp(int port, int timeOut) :_timeOut(timeOut)
	{
		initialise(port);
		_contcp = nullptr;
		_paralelAccept.startThread(paralelAccept, this);
	}

	int tryAccept(MyQueue<T> *sendQueue, MyQueue<T> *recieveQueue)
	{
		if (!_socketConnectionsQueue.empty()) 
		{
			SOCKET connectedSocket = _socketConnectionsQueue.front();
			_socketConnectionsQueue.pop();
			_contcp = new ConnectionTCP<T>(connectedSocket, sendQueue, recieveQueue);
			_conSocketWorkin.startThread(tcpWorkin, this);
			_conSocketWorkout.startThread(tcpWorkout, this);
			return 0;
		}
		return -1;
	}

	int forceAccept(MyQueue<T> *sendQueue, MyQueue<T> *recieveQueue)
	{
		while (_socketConnectionsQueue.empty()) 
		{
			Sleep(100);
		}
		SOCKET connectedSocket = _socketConnectionsQueue.front();
		_socketConnectionsQueue.pop();
		_contcp = new ConnectionTCP<T>(connectedSocket, sendQueue, recieveQueue);
		_conSocketWorkin.startThread(tcpWorkin, this);
		_conSocketWorkout.startThread(tcpWorkout, this);
		return 0;
	}

	void supportOneConnection(MyQueue<T> *sendQueue, MyQueue<T> *recieveQueue)
	{
		_oneConnection.startThread(oneConnection, this, sendQueue, recieveQueue);

	}

	void closeServer()
	{
		_conSocketWorkin.join();
		_conSocketWorkout.join();
		if (_contcp != nullptr)
		{
			delete _contcp;
			_contcp = nullptr;
		}
	}

	~ServerTcp()
	{
		_paralelAccept.join();
		_oneConnection.join();
		closeServer();
	}

};

#endif