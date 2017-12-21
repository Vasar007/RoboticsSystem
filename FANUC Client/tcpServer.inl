#ifndef TCP_SERVER_INL
#define TCP_SERVER_INL

#include<Ws2tcpip.h>

#include "SocketWorking.hpp"

static addrinfo* SERVER_ADDR22;

template <typename T>
int ServerTCP<T>::initialise(int port)
{
    // Luanching main SOCKET.
	_statusField.setObject("intialising");
	_sockMain = SocketWorking::getInstance().getFreeSocket();
	if (_sockMain == INVALID_SOCKET)
	{
		return WSAGetLastError();
	}

    // Creating address.
	sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(static_cast<u_short>(port));
	destAddr.sin_addr.s_addr = INADDR_ANY;

    // binding socket
	if (bind(_sockMain, reinterpret_cast<sockaddr *>(&destAddr), sizeof(destAddr)) < 0)
	{
		closesocket(_sockMain);
		_sockMain = INVALID_SOCKET;
		return WSAGetLastError();
	}

	// let socket be connetable
	if (listen(_sockMain, SOMAXCONN) < 0)
	{
		closesocket(_sockMain);
		_sockMain = INVALID_SOCKET;
		return WSAGetLastError();
	}

	// cleaning addres
	freeaddrinfo(SERVER_ADDR22);
	
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
	ins->_prevRecieveTime = clock();
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
		{
			ins->_prevRecieveTime = clock();
		}

		if (clock() - ins->_prevRecieveTime > ins->_timeOut)
		{
			ins->_connected = false;
		}

		Sleep(10);
	}
}

template <typename T>
void ServerTCP<T>::oneConnectionSupportThread(std::mutex* mt, bool* f, ServerTCP* instance,
    MyQueue<T>* sendQueue, MyQueue<T>* recieveQueue)
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
	
	    if (!instance->_socketConnectionsQueue.isEmpty())
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
    // Set SOCKET in non-blocking mode.
	unsigned long value = 1;
	ioctlsocket(instance->_sockMain, FIONBIO, &value);

    // Collection of SSOCKETs.
	fd_set sSet = { 1,{ instance->_sockMain } };
	
    // Time structure.
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

        // Set SOCKET.
		sSet.fd_count = 1;
		
	    // Check select.
	    const int selectRes = select(0, &sSet, nullptr, nullptr, &timeout);
		if (selectRes == SOCKET_ERROR)
		{
			return;
		}
		
        // If there is read SOCKET.
	    if (selectRes)
		{
			SOCKADDR_IN nsa;
			int sizeofNsa = sizeof(nsa);

			const SOCKET connectedSocket = accept(instance->_sockMain, reinterpret_cast<SOCKADDR *>(&nsa), &sizeofNsa);
			if (connectedSocket == INVALID_SOCKET)
			{
				continue;
			}
			instance->_socketConnectionsQueue.push(connectedSocket);
		}

		Sleep(10);
	}
}

template <typename T>
ServerTCP<T>::ServerTCP(int port, int timeOut)
:_timeOut(timeOut), _statusField("Clients status: ", "no connection")
{
    const int res = initialise(port);
	if(res)
	{
        throw "WinSock error: " + std::to_string(res);
	}
	_curConnectedClient = nullptr;
	_parallelAccept.startThread(paralelAcceptThread, this);
}

template <typename T>
int ServerTCP<T>::tryAccept(MyQueue<T>* sendQueue, MyQueue<T>* recieveQueue)
{
	_statusField.setObject("trying accept");
	if (!_socketConnectionsQueue.isEmpty())
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
	while (_socketConnectionsQueue.isEmpty())
	{
		Sleep(_timeOut);
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
	_supportOneConnection.startThread(oneConnectionSupportThread, this, sendQueue, recieveQueue);
}

template <typename T>
void ServerTCP<T>::stopServer()
{
	_coordsInputStream.closeThread();
	_coordsOuputStream.closeThread();
	if (_curConnectedClient != nullptr)
	{
		delete _curConnectedClient;
		_curConnectedClient = nullptr;
	}
}

template <typename T>
ServerTCP<T>::~ServerTCP()
{
	_parallelAccept.closeThread();
	_supportOneConnection.closeThread();
	stopServer();
}
#endif // TCP_SERVER_INL
