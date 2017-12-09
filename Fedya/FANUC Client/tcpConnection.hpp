#ifndef TCP_CONNECTION
#define TCP_CONNECTION
#pragma once

#include <WinSock2.h>
#include "MyQueue.hpp"

/**
 * \brief Class for processing clint.
 * \tparam T Type of cord which we recive and send.
 */
template<typename T>
class ConnectionTCP
{
	/**
	 * \brief SOCKET from which we comunicate.
	 */
	SOCKET _soc;

	/**
	 * \brief Queue of points which we send.
	 */
	MyQueue<T>* _sendQueue;
	
	/**
	 * \brief Queue of points which we recive.
	 */
	MyQueue<T>* _reciveQueue;
	
	/**
	 * \brief String of resived bytes which we parse to take points.
	 */
	std::string _stringBuffer;

	myInterface::StaticField<int> _pocketsRecived;

public:
	/**
	 * \brief Default constructor.
	 */
	ConnectionTCP() = default;

	/**
	 * \brief Constructor for starting workig.
	 * \param soc SOCKET for comunication.
	 * \param sendQueue Queue for sending.
	 * \param reciveQueue Queue for reciving.
	 */
	ConnectionTCP(SOCKET soc, MyQueue<T>* sendQueue, MyQueue<T>* reciveQueue);

	/**
	 * \brief Function for sending font point from sending queue to client.
	 */
	void sendCoord();

	/**
	 * \brief Function for reciving point from client and adding to recvqueue.
	 * \return 0 if point was recived,
	 *  1 if SOCKET haven't recive any pocket,
	 *  -15 if was winSock error.
	 */
	int recvCoord();
};

#include "tcpConnectionDifinition.inl"

#endif
