#ifndef ROBOT_CONNECT
#define ROBOT_CONECT
#pragma once

#include <fstream>
#include <string>
#include "MyShower.hpp"
#include "MyQueue.hpp"
#include "SocketWorking.hpp"
#include "MyThread.hpp"
#include "RobotSend.hpp"
#include "RobotRecieve.hpp"

#define DEFAULT_BUFLEN 48

template<typename T>
class RobotConnect
{
	int _ports, _portr; //порты для отправки и принятия
	std::string _serverAddrString;//строка для IP сервера
	int _segtime;//время перемещения из одной точки в другую в мс
	double _robotSpeed;//рассчетная скорость робота
	int _syscoord;//тип системы координат
	// 0 - JOINT | 2 - WORLD
	int _disconnectTime1, _disconnectTime2;//время разрыва связи сервера и клиента

	SOCKET _sockSend, _sockRecv; //создаём сокет приема и отправки

	MyQueue<T>* _sendingQueue, *_recivingQueue;//очереди точек на отправку и прием

	MyQueue<T> _cloneQueue;//очередь точек на обработке у робота

	RobotSend<T> *_robotSend;//структура реализующая отправку кординат

	RobotRecieve<T> *_robotRecieve;//структура реализующая прием кординат

	MyThread _threadSend;//поток паралкльного отправки кординат на робота

	MyThread _threadRecv;//поток получения кординат с робота и переноса их в буфер передачи

	MyThread _mainThread;//главный поток обработки

	bool _connected;
	

	int healServerSecondConnection() const
	{
		char buffer[128];
		ZeroMemory(buffer, 128);

		SOCKET sockrecv = SocketWorking::getInstance().connectToRobotServer(_serverAddrString.c_str(), _portr, _disconnectTime2);
		
		if (sockrecv == INVALID_SOCKET)
		{
			sprintf_s(buffer, "recive socket error %d\0", WSAGetLastError());
			myInterface::MyShower::getInstance().show(buffer);
			return 1;
		}
		SocketWorking::getInstance().deleteSocket(sockrecv);
		return 0;
	}

	int conRobot()
	{
		char buffer[128];
		
		//создание сокеа отправки
		_sockSend = SocketWorking::getInstance().connectToRobotServer(_serverAddrString.c_str(), _ports, _disconnectTime2);
		if (_sockSend == INVALID_SOCKET)
		{
			sprintf_s(buffer, "send socket error %d\0", WSAGetLastError());
			myInterface::MyShower::getInstance().show(buffer);
			return 1;
		}

		//содание сокета приема
		_sockRecv = SocketWorking::getInstance().connectToRobotServer(_serverAddrString.c_str(), _portr, _disconnectTime2);
		if (_sockRecv == INVALID_SOCKET) 
		{
			sprintf_s(buffer, "recive socket error %d\0", WSAGetLastError());
			myInterface::MyShower::getInstance().show(buffer);
			return 2;
		}

		return 0;
	}

	static void reverseStream(std::mutex *mt, bool *f, RobotConnect *ins) 
	{
		long long prevConnetectedTime = clock();
		bool wasFirstPoint = false;
		T rc;
		int was = 1, sleepBonus = 0;
		ins->_connected = true;
		while (true) 
		{
			if (ins->_robotRecieve->readCoord() == -15) {
				if ((clock() - prevConnetectedTime > ins->_disconnectTime2 + sleepBonus) && wasFirstPoint) 
				{
					ins->_forcedRestart = true;
				}//*/
				if ((clock() - prevConnetectedTime > ins->_disconnectTime1*was + sleepBonus) && wasFirstPoint) 
				{
					ins->_connected = false;
					if (ins->_cloneQueue.empty()) 
					{
						ins->_robotSend->sendPrevCoord();
					}
					else 
					{
						sleepBonus = ins->_robotRecieve->_prevCoord.difference(ins->_cloneQueue.front()) / ins->_robotSpeed;
					}

#ifdef LOCAL_DEBUG
					std::ofstream out("look points.txt", std::ofstream::app);
					out << sendbuf << "1\n";
					out.close();//*/
#endif // LOCAL_DEBUG

					was++;
				}//*/

			}
			else {
				//sscanf_s(recvbuf, "%d %d %d %d %d %d %d", &rc.Xr, &rc.Yr, &rc.Zr, &rc.Uw, &rc.Up, &rc.Uz, &rc.segTime);

				

				ins->_connected = true;
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

	static void sendParalel(std::mutex *mt, bool *f, RobotConnect *ins) 
	{

		ins->_robotSend->resendCloneQueue();

		while (true) 
		{
			if (ins->_connected)
			{
				const std::pair<bool, RobotCoord> tryPull = ins->_sendingQueue->pull();
				if (tryPull.first)
					ins->_robotSend->sendRobotCoord(tryPull.second);//отправляем буфер
			}
			mt->lock();
			if (!(*f)) 
			{
				mt->unlock();
				if (ins->_connected) 
				{
					while (!ins->_sendingQueue->empty()) {
						const RobotCoord rc = ins->_sendingQueue->front();
						ins->_sendingQueue->pop();
						ins->_robotSend->sendRobotCoord(rc);//отправляем буфер
					}
					Sleep(1000);
				}
				break;
			}
			mt->unlock();
			Sleep(ins->_segtime);
		}
	}

	int startWorking()
	{
		const int iResult = conRobot();
		if (iResult) 
		{
			std::cout << "Conection error " << iResult << " " << WSAGetLastError() << std::endl;
			closesocket(_sockSend);
			closesocket(_sockRecv);
			return -4;
		}
		/// подлючение к роботу

		//type of cordiates:0 = JOINT | 1 = JOGFRAME | 2 = WORLDFRAME | 3 = TOOLFRAME | 4 = USER FRAME
		char coord[3] = { 0,0,0 };
		sprintf_s(coord, "%d ", _syscoord);
		if (send(_sockSend, coord, 1, 0) == SOCKET_ERROR) {
			std::cout << "Conection error " << WSAGetLastError() << std::endl;
			closesocket(_sockSend);
			return -5;
		}//отправляем буфер
		 ///выбор системы координат

		_robotSend = new RobotSend<T>(_sockSend, &_cloneQueue);
		_robotRecieve = new RobotRecieve<T>(_sockRecv, &_cloneQueue, _recivingQueue, _disconnectTime1/3);

		_threadSend.startThread(sendParalel, this);

		_threadRecv.startThread(reverseStream, this);

		return 0;
	}

	void tryConnect() 
	{
		int steps = 0;
		while (startWorking() < 0)
		{
			myInterface::MyShower::getInstance().show("reconecting");
			Sleep(_disconnectTime2);
			myInterface::MyShower::getInstance().update();
			++steps;
			if (steps >= 3) 
			{
				healServerSecondConnection();
				steps = 0;
			}
		}
	}

	void finishWorking()
	{
		_threadSend.join();
		_threadRecv.join();

		SocketWorking::getInstance().deleteSocket(_sockSend);
		SocketWorking::getInstance().deleteSocket(_sockRecv);

		delete _robotSend;
		delete _robotRecieve;
		_robotSend = nullptr;
		_robotRecieve = nullptr;
	}


	void restart()
	{
		finishWorking();

		tryConnect();

		myInterface::MyShower::getInstance().update();
	}

	static void mainLoop(std::mutex *mt, bool *f,RobotConnect *ins)
	{
		ins->tryConnect();

		while(0==0)
		{
			mt->lock();
			if(!*f)
			{
				mt->unlock();
				ins->finishWorking();
				break;
			}
			mt->unlock();

			if(ins->_forcedRestart)
			{
				ins->restart();
				ins->_forcedRestart = false;
			}

			Sleep(ins->_segtime);
		}
	}

public:

	bool _forcedRestart;

	RobotConnect(std::string configFileName, MyQueue<T> *sendingQueue, MyQueue<T> *recivingQueue):
	_sendingQueue(sendingQueue),_recivingQueue(recivingQueue)
	{
		_portr = _disconnectTime1 = _disconnectTime2 = _ports = _robotSpeed = _segtime = 
		_syscoord = -1;
		_sockRecv = _sockSend = INVALID_SOCKET;
		_connected = _forcedRestart = false;
		_robotSend = nullptr;
		_robotRecieve = nullptr;
		
		std::ifstream in(configFileName);
		if(!in)
		{
			myInterface::MyShower::getInstance().show("cannot open file: " + configFileName);
			std::exception exp("cannot open file");
			throw exp;
		}

		
		in >> _serverAddrString ///считываем IP сервера
			>> _ports >> _portr		 /// счиьывае номера портов
			>> _segtime				 /// время перехода из одной точки в другую
			>> _robotSpeed			 ///расчетная скорость передвижения между точками
			>> _syscoord				 /// тип системы кординат
			>> _disconnectTime1		 /// время после, которого проверяется подключение
			>> _disconnectTime2;		 /// время после которого происходит переподключение

		in.close();
	}

	void startMainLoop()
	{
		_mainThread.startThread(mainLoop, this);
	}

	void stopMainLoop()
	{
		_mainThread.join();
	}

	void restartMainLoop()
	{
		stopMainLoop();
		startMainLoop();
	}

	~RobotConnect()
	{
		_mainThread.join();
	}
};

#endif // ROBOT_CONNECT
