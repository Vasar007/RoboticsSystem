#pragma once

#ifndef TCP_CONNECTION

#define TCP_CONNECTION

#include <WinSock2.h>
#include "MyQueue.hpp"

/**
 * \brief class for processing clint
 * \tparam T type of cord which we recive and send
 */
template<typename T>
class ConnectionTCP
{
	/**
	 * \brief SOCKET from which we comunicate
	 */
	SOCKET _soc;

	/**
	 * \brief queue of points which we send
	 */
	MyQueue<T>* _sendQueue;
	
	/**
	 * \brief queue of points which we recive
	 */
	MyQueue<T>* _reciveQueue;
	
	/**
	 * \brief string of resived bytes which we parse to take points
	 */
	std::string _sbuf;

public:
	/**
	 * \brief default constructor
	 */
	ConnectionTCP() = default;

	/**
	 * \brief constructor for starting workig
	 * \param soc SOCKET for comunication
	 * \param sendQueue queue for sending
	 * \param reciveQueue queue for reciving
	 */
	ConnectionTCP(SOCKET soc, MyQueue<T>* sendQueue, MyQueue<T>* reciveQueue);

	/**
	 * \brief function for sending font point from sending queue to client
	 */
	void sendCoord();

	/**
	 * \brief function for reciving point from client and adding to recvqueue
	 * \return 0 if point was recived, 1 if SOCKET haven't recive any pocket, -15 if was winSock error
	 */
	int recvCoord();
};

#include "tcpConnectionDifinition.hpp"

#endif
