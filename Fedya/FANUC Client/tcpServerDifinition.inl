#ifndef TCP_SERVER_DIFINTION
#define TCP_SERVER_DIFINTION
#pragma once

#include<Ws2tcpip.h>
#include "SocketWorking.hpp"

static addrinfo* serverAddr22;

template <typename T>
int ServerTCP<T>::initialise(int port)
{
	_statusField.setObject("intialising");
	_sockMain = SocketWorking::getInstance().getFreeSocket();
	if (_sockMain == INVALID_SOCKET)
	{
		//freeaddrinfo(server_addr);
		return WSAGetLastError();
	}
	//creating socket

	sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(static_cast<u_short>(port));
	destAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(_sockMain, reinterpret_cast<sockaddr *>(&destAddr), sizeof(destAddr)) < 0)
	{
		closesocket(_sockMain);
		_sockMain = INVALID_SOCKET;
		std::cout << WSAGetLastError();
		return WSAGetLastError();
	}
	//binding socket

	if (listen(_sockMain, SOMAXCONN) < 0)
	{
		closesocket(_sockMain);
		_sockMain = INVALID_SOCKET;
		return WSAGetLastError();
	}
	//let socket be connetable

	freeaddrinfo(serverAddr22);
	//cleaning addres
	return 0;
}

template <typename T>
void ServerTCP<T>::recieveThread(std::mutex* mt, bool* f, ServerTCP* ins)
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

		ins->_curConnectedClient->sendCoord();

		Sleep(10);
	}
}

template <typename T>
void ServerTCP<T>::sendThread(std::mutex* mt, bool* f, ServerTCP* ins)
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

		if (ins->_curConnectedClient->recvCoord() == 0)
			ins->_prevRecieve = clock();

		if (clock() - ins->_prevRecieve > ins->_timeOut)
			ins->_connected = false;

		Sleep(10);
	}
}

template <typename T>
void ServerTCP<T>::oneConnectionThread(std::mutex* mt, bool* f, ServerTCP* instance, MyQueue<T>* sendQueue,
	MyQueue<T>* recieveQueue)
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
			instance->stopServer();
			instance->forceAccept(sendQueue, recieveQueue);
			instance->_connected = true;
		}
		Sleep(10);
	}
}

template <typename T>
void ServerTCP<T>::paralelAcceptThread(std::mutex* mt, bool* f, ServerTCP* instance)
{
	unsigned long value = 1;
	ioctlsocket(instance->_sockMain, FIONBIO, &value);

	fd_set sSet = { 1,{ instance->_sockMain } };
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
			return;
		}
		if (selectRes)
		{
			SOCKADDR_IN nsa;
			int sizeofNsa = sizeof(nsa);

			const SOCKET connectedSocket = accept(instance->_sockMain, reinterpret_cast<SOCKADDR *>(&nsa), &sizeofNsa);
			if (connectedSocket == INVALID_SOCKET)
				continue;
			instance->_socketConnectionsQueue.push(connectedSocket);
		}
		Sleep(10);
	}
}

template <typename T>
ServerTCP<T>::ServerTCP(int port, int timeOut)
:_timeOut(timeOut), _statusField("Clients status: ", "no connection")
{
	initialise(port);
	_curConnectedClient = nullptr;
	_parallelAccept.startThread(paralelAcceptThread, this);
}

template <typename T>
int ServerTCP<T>::tryAccept(MyQueue<T>* sendQueue, MyQueue<T>* recieveQueue)
{
	_statusField.setObject("trying accept");
	if (!_socketConnectionsQueue.empty())
	{
		SOCKET connectedSocket = _socketConnectionsQueue.front();
		_socketConnectionsQueue.pop();
		_curConnectedClient = new ConnectionTCP<T>(connectedSocket, sendQueue, recieveQueue);
		_coordsInputStream.startThread(recieveThread, this);
        _coordsOuputStream.startThread(sendThread, this);
		return 0;
	}
	return -1;
}

template <typename T>
int ServerTCP<T>::forceAccept(MyQueue<T>* sendQueue, MyQueue<T>* recieveQueue)
{
	_statusField.setObject("waitingconnection");
	while (_socketConnectionsQueue.empty())
	{
		Sleep(100);
	}
	SOCKET connectedSocket = _socketConnectionsQueue.front();
	_socketConnectionsQueue.pop();
	_curConnectedClient = new ConnectionTCP<T>(connectedSocket, sendQueue, recieveQueue);
	_coordsInputStream.startThread(recieveThread, this);
	_coordsOuputStream.startThread(sendThread, this);
	_statusField.setObject("connected");
	return 0;
}

template <typename T>
void ServerTCP<T>::supportOneConnection(MyQueue<T>* sendQueue, MyQueue<T>* recieveQueue)
{
	_supportOneConnection.startThread(oneConnectionThread, this, sendQueue, recieveQueue);
}

template <typename T>
void ServerTCP<T>::stopServer()
{
	_coordsInputStream.stopThread();
	_coordsOuputStream.stopThread();
	if (_curConnectedClient != nullptr)
	{
		delete _curConnectedClient;
		_curConnectedClient = nullptr;
	}
}

template <typename T>
ServerTCP<T>::~ServerTCP()
{
	_parallelAccept.stopThread();
	_supportOneConnection.stopThread();
	stopServer();
}
#endif
