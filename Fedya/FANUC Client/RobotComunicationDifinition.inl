#ifndef ROBOT_COMUNICATION_DIFINTION
#define ROBOT_COMUNICATION_DIFINTION
#pragma once

#include <fstream>

#include "MyShower.hpp"
#include "SocketWorking.hpp"
#include "RobotCoord.hpp"

template <typename T>
int RobotConnect<T>::healServerSecondConnection() const
{

	SOCKET sockrecv = SocketWorking::getInstance().getConnectedSocket(_serverAddrString.c_str(), _recvPort,
		_timeBetweenReconnection);

	if (sockrecv == INVALID_SOCKET)
	{
		myInterface::MyShower::getInstance().addLog("recive socket error ", WSAGetLastError());
		return 1;
	}
	SocketWorking::getInstance().deleteSocket(sockrecv);
	return 0;
}

template <typename T>
void RobotConnect<T>::recivingStream(std::mutex* mt, bool* f, RobotConnect* ins)
{
	long long prevConnetectedTime = clock();
	bool wasFirstPoint = false;
	T rc;
	int was = 1, sleepBonus = 0;
	ins->_connected = true;
	while (true)
	{
		if (ins->_robotRecieve->readCoord() == -15)
		{
			if ((clock() - prevConnetectedTime > ins->_timeBetweenReconnection + sleepBonus) && wasFirstPoint)
			{
				ins->_forcedRestart = true;
			}
			if ((clock() - prevConnetectedTime > ins->_timeBetweenEchoConnectionCheking * was + sleepBonus) && wasFirstPoint)
			{
				ins->_connected = false;
				ins->_connectionField.setObject("Unanswered");
				if (ins->_cloneQueue.empty())
				{
					ins->_robotSend->sendPrevCoord();
				}
				else
				{
					sleepBonus = static_cast<int>(ins->_robotRecieve->_prevCoord.difference(ins->_cloneQueue.front()) / ins->_robotAssumingSpeed);
				}
				was++;
			}
		}
		else
		{
			ins->_connected = true;
			ins->_connectionField.setObject("connected");
			prevConnetectedTime = clock();
			was = 1;
			wasFirstPoint = true;
		}
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
		if (ins->_connected)
		{
			const std::pair<bool, RobotCoord> tryPull = ins->_sendingQueue->tryPull();
			if (tryPull.first)
				ins->_robotSend->moveCoord(tryPull.second);//отправляем буфер
		}
		mt->lock();
		if (!(*f))
		{
			mt->unlock();
			if (ins->_connected)
			{
				while (!ins->_sendingQueue->empty())
				{
					const RobotCoord rc = ins->_sendingQueue->front();
					ins->_sendingQueue->pop();
					ins->_robotSend->moveCoord(rc);//отправляем буфер
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
int RobotConnect<T>::beginConnection()
{
	//создание сокета отправки
	_sockSend = SocketWorking::getInstance().getConnectedSocket(_serverAddrString.c_str(), _sendPort, _timeBetweenReconnection);
	if (_sockSend == INVALID_SOCKET)
	{
		myInterface::MyShower::getInstance().addLog("send socket error ", WSAGetLastError());
		return 1;
	}

	//содание сокета приема
	_sockRecv = SocketWorking::getInstance().getConnectedSocket(_serverAddrString.c_str(), _recvPort, _timeBetweenReconnection);
	if (_sockRecv == INVALID_SOCKET)
	{
		myInterface::MyShower::getInstance().addLog("recive socket error ", WSAGetLastError());
		closesocket(_sockSend);
		return 2;
	}

	/// подлючение к роботу

	//type of cordiates:0 = JOINT | 1 = JOGFRAME | 2 = WORLDFRAME | 3 = TOOLFRAME | 4 = USER FRAME
	char coord[3] = { 0,0,0 };
	sprintf_s(coord, "%d ", _sysCoord);
	if (send(_sockSend, coord, 1, 0) == SOCKET_ERROR)
	{
		//std::cout << "Conection error " << WSAGetLastError() << std::endl;
		myInterface::MyShower::getInstance().addLog("Sending syscord error: ", WSAGetLastError());
		closesocket(_sockSend);
		return -5;
	}//отправляем буфер
	 ///выбор системы координат

	_robotSend = new RobotSend<T>(_sockSend, &_cloneQueue);
	_robotRecieve = new RobotRecieve<T>(_sockRecv, &_cloneQueue, _recivingQueue, _timeBetweenEchoConnectionCheking / 3);

	_threadSend.startThread(sendingStream, this);

	_threadRecv.startThread(recivingStream, this);

	return 0;
}

template <typename T>
void RobotConnect<T>::tryConnect()
{
	int steps = 0;
	while (beginConnection() < 0)
	{
		_connectionField.setObject("connecting");
		Sleep(_timeBetweenReconnection);
		++steps;
		if (steps >= 3)
		{
			healServerSecondConnection();
			steps = 0;
		}
	}
}

template <typename T>
void RobotConnect<T>::endConnection()
{
	_threadSend.stopThread();
	_threadRecv.stopThread();

	SocketWorking::getInstance().deleteSocket(_sockSend);
	SocketWorking::getInstance().deleteSocket(_sockRecv);

	delete _robotSend;
	delete _robotRecieve;
	_robotSend = nullptr;
	_robotRecieve = nullptr;
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
RobotConnect<T>::RobotConnect(std::string configFileName, MyQueue<T>* sendingQueue, MyQueue<T>* recivingQueue) :
	_sendingQueue(sendingQueue), _recivingQueue(recivingQueue),_cloneQueue("clone queue"),_connectionField("FANUC M20iA ","disconneted")
{
	_sockRecv = _sockSend = INVALID_SOCKET;
	_connected = _forcedRestart = false;
	_robotSend = nullptr;
	_robotRecieve = nullptr;

	std::ifstream in(configFileName);
	if (!in)
	{
		myInterface::MyShower::getInstance().addLog("cannot open file: ", configFileName);
		std::exception exp("cannot open file");
		throw exp;
	}


	in >> _serverAddrString ///считываем IP сервера
		>> _sendPort >> _recvPort /// счиьывае номера портов
		>> _segtime /// время перехода из одной точки в другую
		>> _robotAssumingSpeed ///расчетная скорость передвижения между точками
		>> _sysCoord /// тип системы кординат
		>> _timeBetweenEchoConnectionCheking /// время после, которого проверяется подключение
		>> _timeBetweenReconnection; /// время после которого происходит переподключение

	in.close();
}

template <typename T>
void RobotConnect<T>::startMainLoop()
{
	_mainThread.startThread(mainLoop, this);
}

template <typename T>
void RobotConnect<T>::stopMainLoop()
{
	_mainThread.stopThread();
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
	_mainThread.stopThread();
}

#endif