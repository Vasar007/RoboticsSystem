#pragma once

#ifndef FANUC_COMMUNICATION

#define FANUC_COMMUNICATION

#include"RobotCoord.hpp"
#include <mutex>
#include "MyQueue.hpp"
#include <winsock2.h>
#include "SocketWorking.hpp"


#define DEFAULT_BUFLEN 48
#define SENDBUF 48 //длина буфера для отправки

static bool forcedRestart = false;
static int disconnectTime1, disconnectTime2;

//функция приема
void reverseStream(std::mutex *mt, bool *f, ///переменные для завершения функции
	int *segtime, ///период отправки
	double robotSpeed, ///рассчетная скорость робота
	SOCKET sockrecv, SOCKET mySock, ///сокеты приема и отправки 
	MyQueue<RobotCoord> *fanucReverseQueue, ///буфер приема кординат
	MyQueue<RobotCoord> *cloneQueue) {///копия очереди отправленных точек
	char recvbuf[DEFAULT_BUFLEN];
	long long prevConnetectedTime = clock();
	RobotCoord rc;
	int was = 1, sleepBonus = 0;
	bool wasFirstPoint = false;
	std::string sbuf, str;
	connected = true;
	while (0 == 0) {
		ZeroMemory(recvbuf, DEFAULT_BUFLEN);
		if (recvspam(sockrecv, recvbuf, DEFAULT_BUFLEN - 1) == -15) {
			if ((clock() - prevConnetectedTime > disconnectTime2 + sleepBonus) && wasFirstPoint) {
				forcedRestart = true;
			}//*/
			if ((clock() - prevConnetectedTime > disconnectTime1*was + sleepBonus) && wasFirstPoint) {
				connected = false;
				if (cloneQueue->empty()) {
					sendPredRobotCoord(mySock, cloneQueue);
				}
				else {
					sleepBonus = rc.difference(cloneQueue->front()) / robotSpeed;
				}

#ifdef LOCAL_DEBUG
				std::ofstream out("look points.txt", std::ofstream::app);
				out << sendbuf << "1\n";
				out.close();//*/
#endif // LOCAL_DEBUG

				was++;
			}//*/
			if (!wasFirstPoint) {
				sendPredRobotCoord(mySock, cloneQueue);
			}

		}
		else {
			//sscanf_s(recvbuf, "%d %d %d %d %d %d %d", &rc.Xr, &rc.Yr, &rc.Zr, &rc.Uw, &rc.Up, &rc.Uz, &rc.segTime);

			sbuf += recvbuf;
			int coordsNumber = 0;
			for (int i = 0;i < sbuf.size();i++) {
				if (sbuf[i] != ' ') {

					while (i < sbuf.size() && sbuf[i] != ' ')
						i++;
					if (i < sbuf.size() && sbuf[i] == ' ')
						++coordsNumber;
				}
				if (coordsNumber == 7) {
					sscanf_s(sbuf.c_str(), "%d %d %d %d %d %d %d", &rc._xr, &rc._yr, &rc._zr, &rc._uw, &rc._up, &rc._uz, &rc._segTime);
					rc._typeOfMoving = 0;
					rc._control = 0;
					fanucReverseQueue->push(rc);
					if (!cloneQueue->empty()) {
						cloneQueue->pop();
					}
					sbuf = sbuf.substr(i);
					i = 0;
					coordsNumber = 0;
				}
			}

			connected = true;
			prevConnetectedTime = clock();
			was = 1;
			wasFirstPoint = true;
		}
		mt->lock();
		if (!(*f)) {
			mt->unlock();
			break;
		}
		mt->unlock();
		Sleep(*segtime);
	}
}

///функция для паралельного отправления точек на робот
void sendParalel(std::mutex *mt, bool *f, int *segtime, SOCKET mySock, MyQueue<RobotCoord> *fanucSendQueue,
	MyQueue<RobotCoord> *cloneQueue) {

#ifdef WITH_POINTS_SAVER

	std::ifstream insertSavingStream("saver.txt");
	while (insertSavingStream.peek() != EOF) {
		int tttt;
		RobotCoord rc;
		insertSavingStream >> tttt >> rc;
		if (tttt == 1)
			sendClone.push(std::make_pair(1, rc));
	}
	insertSavingStream.close();
	std::ofstream outmp("saver.txt");
	outmp.close();

#endif
	MyQueue<RobotCoord> tmpCloneQueue;
	if (!cloneQueue->empty()) {
		tmpCloneQueue.swap((*cloneQueue));
	}
	while (!tmpCloneQueue.empty()) {
		const RobotCoord rc = tmpCloneQueue.front();
		tmpCloneQueue.pop();
		sendRobotCoord(mySock, rc, cloneQueue);
	}

	while (0 == 0) {
		if (connected) {
			const std::pair<bool, RobotCoord> tryPull = fanucSendQueue->pull();
			if (tryPull.first) {
				sendRobotCoord(mySock, tryPull.second, cloneQueue);//отправляем буфер
				*segtime = tryPull.second._segTime;
#ifdef LOCAL_DEBUG
				std::ofstream out("look points.txt", std::ofstream::app);
				out << sendbuf << "1\n";
				out.close();//*/
#endif // LOCAL_DEBUG

				//-------------------------------------------------------------------------
			}
		}
		mt->lock();
		if (!(*f)) {
			mt->unlock();
			if (connected) {
				while (!fanucSendQueue->empty()) {
					const RobotCoord rc = fanucSendQueue->front();
					fanucSendQueue->pop();
					sendRobotCoord(mySock, rc, cloneQueue);//отправляем буфер
				}
				Sleep(1000);
			}
			/*else {
			std::ofstream out("saver.txt");
			std::queue<std::pair<bool, RobotCoord>> tmpq2;
			std::vector<RobotCoord> list = buf->pullAll();
			for (int i = 0;i < list.size();i++) {
			prevCoord = list[i];
			out << 1 << " " << prevCoord.toString() << "\n";
			}
			while (!sendClone.empty()) {
			if (sendClone.front().first) {
			out << 2 << " " << sendClone.front().second.toString() << "\n";
			tmpq2.push(sendClone.front());
			}
			sendClone.pop();
			}
			tmpq2.Swap(sendClone);
			out.close();
			}//*/
			break;
		}
		mt->unlock();
		Sleep(*segtime);
	}
}


#endif