#ifndef ROBOT_COMUNICATION_INL
#define ROBOT_COMUNICATION_INL

#include <fstream>

#include "MyShower.hpp"
#include "SocketWorking.hpp"
#include "RobotCoord.hpp"

template <typename T>
int RobotConnect<T>::launchReceivingSocketAndClosingIt() const
{
    // Launching receiving SOCKET.
	SOCKET sockrecv = SocketWorking::getInstance().getConnectedSocket(_serverAddressString.c_str(),
        _recvPort,_timeBetweenReconnection);

	if (sockrecv == INVALID_SOCKET)
	{
        // if connection refused -> everything works right.
        if(WSAGetLastError() == WSAETIMEDOUT || WSAGetLastError() == WSAECONNREFUSED)
		    return 0;

        // If another error -> return it.
        return WSAGetLastError();
	}

    // If connection succeed -> server work incorrectly, but now we fix it.
    myInterface::MyShower::getInstance().addLog("server error \"second SOCKET waiting\"");

	SocketWorking::getInstance().deleteSocket(sockrecv);
	return 1;
}

template <typename T>
void RobotConnect<T>::receivingStream(std::mutex* mt, bool* f, RobotConnect* ins)
{
	// Time when previous point was received.
    clock_t prevConnetectedTime = clock();
	
    // Flag if any coordinate was received.
    bool wasFirstPoint = false;
    
    // How many echo-coordinates was sent.
    int was = 0;
    
    // Additional time for receiving answer.
    int sleepBonus = 0;
	
    ins->_connected = true;
	
    while (true)
	{
		if (ins->_robotReceive->receiveCoord() == -15)
		{
            // Initializing re-connection.
			if ((clock() - prevConnetectedTime > ins->_timeBetweenReconnection + sleepBonus) 
                && wasFirstPoint)
			{
				ins->_forcedRestart = true;
			}

            // Sending previous coordinate for checking connection.
			if ((clock() - prevConnetectedTime > ins->_timeBetweenEchoConnectionChecking * was 
                + sleepBonus) && wasFirstPoint)
			{
				ins->_connected = false;
				ins->_statusField.setObject("Unanswered");
				if (ins->_cloneQueue.isEmpty())
				{
					ins->_robotSend->sendPreviousCoord();
				}
				else
				{
					sleepBonus = static_cast<int>(ins->_robotReceive->_prevCoord.difference(
                        ins->_cloneQueue.front()) / ins->_robotAssumingSpeed);
				}
				was++;
			}
		}
		else
		{
			ins->_connected = true;
			ins->_statusField.setObject("connected");
			prevConnetectedTime = clock();
			was = 1;
			wasFirstPoint = true;
		}

        // End point.
		mt->lock();
		if (!(*f))
		{
			mt->unlock();
			break;
		}
		mt->unlock();

		Sleep(ins->_segtime);
	}
}

template <typename T>
void RobotConnect<T>::sendingStream(std::mutex* mt, bool* f, RobotConnect* ins)
{
	ins->_robotSend->resendCloneQueue();

	while (true)
	{
        // Trying to get next coordinate and send it.
		if (ins->_connected)
		{
            const std::pair<bool, RobotCoord> tryPull = ins->_sendingQueue->tryPull();
			if (tryPull.first)
			{
				ins->_robotSend->moveRobot(tryPull.second);
			}
		}

        // Ending point.
	    mt->lock();
		if (!(*f))
		{
			mt->unlock();

            // Send all coordinates if we connected.
			if (ins->_connected)
			{
				while (!ins->_sendingQueue->isEmpty())
				{
					const RobotCoord rc = ins->_sendingQueue->front();
					ins->_sendingQueue->pop();
					ins->_robotSend->moveRobot(rc);
				}
				Sleep(1000);
			}
			break;
		}
		mt->unlock();
		Sleep(ins->_segtime);
	}
}

template <typename T>
int RobotConnect<T>::launchConnection()
{
	// Launch sending SOCKET.
	_sockSend = SocketWorking::getInstance().getConnectedSocket(_serverAddressString.c_str(),
        _sendPort, _timeBetweenReconnection);
	if (_sockSend == INVALID_SOCKET)
	{
		myInterface::MyShower::getInstance().addLog("send socket error ", WSAGetLastError());
		return 1;
	}

	// Launch receiving SOCKET.
	_sockRecv = SocketWorking::getInstance().getConnectedSocket(_serverAddressString.c_str(),
        _recvPort, _timeBetweenReconnection);
	if (_sockRecv == INVALID_SOCKET)
	{
		myInterface::MyShower::getInstance().addLog("recive socket error ", WSAGetLastError());
		closesocket(_sockSend);
		return 2;
	}

    // Sending type of coordinates.
	const std::string stringBuffer = std::to_string(_sysCoord);
	if (send(_sockSend, stringBuffer.c_str(), static_cast<int>(stringBuffer.size()), 0)
        == SOCKET_ERROR)
	{
		myInterface::MyShower::getInstance().addLog("Sending syscord error: ",
            WSAGetLastError());
		closesocket(_sockSend);
        closesocket(_sockRecv);
		return -5;
	}
	
    // Start sending and receiving coordinates.
	_robotSend = new RobotSend<T>(_sockSend, &_cloneQueue);
	_robotReceive = new RobotReceieve<T>(_sockRecv, &_cloneQueue, 
        _receivingQueue, _timeBetweenEchoConnectionChecking / 3);

	_threadSend.startThread(sendingStream, this);

	_threadRecv.startThread(receivingStream, this);

	return 0;
}

template <typename T>
void RobotConnect<T>::tryConnect()
{
    // Number of connecting sessions.
	int steps = 0;

    // Endless cycle of trying to launch communication.
	while (launchConnection() < 0)
	{
		_statusField.setObject("connecting");
		Sleep(_timeBetweenReconnection);
		++steps;

        // Every third step try to fix one error with server.
		if (steps >= 3)
		{
			launchReceivingSocketAndClosingIt();
			steps = 0;
		}
	}

    _statusField.setObject("connected");

}

template <typename T>
void RobotConnect<T>::endConnection()
{
    // Close threads.
	_threadSend.closeThread();
	_threadRecv.closeThread();

    // Close SOCKETS.
	SocketWorking::getInstance().deleteSocket(_sockSend);
	SocketWorking::getInstance().deleteSocket(_sockRecv);

    // Delete communicators.
	delete _robotSend;
	delete _robotReceive;
	_robotSend = nullptr;
	_robotReceive = nullptr;
}

template <typename T>
void RobotConnect<T>::restartConnection()
{
	endConnection();

	tryConnect();
}

template <typename T>
void RobotConnect<T>::mainLoop(std::mutex* mt, bool* f, RobotConnect* ins)
{
    // Launch communication.
	ins->tryConnect();

	while (true)
	{
		mt->lock();
		if (!*f)
		{
			mt->unlock();
			ins->endConnection();
			break;
		}
		mt->unlock();

		if (ins->_forcedRestart)
		{
			ins->restartConnection();
			ins->_forcedRestart = false;
		}

		Sleep(ins->_segtime);
	}
}

template <typename T>
RobotConnect<T>::RobotConnect(const std::string& configFileName,
    MyQueue<T>* sendingQueue, MyQueue<T>* recivingQueue) 
    : _sendingQueue(sendingQueue),
      _receivingQueue(recivingQueue),
      _cloneQueue("clone queue"),
      _statusField("FANUC M20iA ","disconneted")
{
    // Trying to open config file.
	std::ifstream inputFileStream(configFileName);
	if (!inputFileStream)
	{
		myInterface::MyShower::getInstance().addLog("cannot open file: ", configFileName);
		std::exception exp("cannot open file");
		throw exp;
	}

    // Inputting parametrs.
    inputFileStream >> _serverAddressString
		>> _sendPort >> _recvPort
		>> _segtime
		>> _robotAssumingSpeed
		>> _sysCoord
		>> _timeBetweenEchoConnectionChecking
		>> _timeBetweenReconnection;

    inputFileStream.close();
}

template <typename T>
void RobotConnect<T>::startMainLoop()
{
	_mainThread.startThread(mainLoop, this);
}

template <typename T>
void RobotConnect<T>::stopMainLoop()
{
	_mainThread.closeThread();
}

template <typename T>
void RobotConnect<T>::restartMainLoop()
{
	stopMainLoop();
	startMainLoop();
}

template <typename T>
RobotConnect<T>::~RobotConnect()
{
	_mainThread.closeThread();
}

#endif // ROBOT_COMUNICATION_INL