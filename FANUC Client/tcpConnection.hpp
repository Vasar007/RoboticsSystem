#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <WinSock2.h>

#include "MyQueue.hpp"

/**
 * \brief       Class for processing client.
 * \tparam T    Type of coordinates which we receive and send.
 */
template<typename T>
class ConnectionTCP
{
private:

	/**
	 * \brief SOCKET for communicating.
	 */
	SOCKET _soc = INVALID_SOCKET;

	/**
	 * \brief Queue of coordinates which we send.
	 */
	MyQueue<T>* _sendQueue = nullptr;
	
	/**
	 * \brief Queue of coordinates which we receive.
	 */
	MyQueue<T>* _receiveQueue = nullptr;
	
	/**
	 * \brief String of reseived bytes for parsing.
	 */
	std::string _stringBuffer = "";

    /**
	 * \brief Field describing number of received coordinates.
	 */
	myInterface::StaticField<int> _pocketsReceived;

public:
	/**
	 * \brief Default constructor.
	 */
	ConnectionTCP() = default;

	/**
	 * \brief                   Constructor for launching working.
	 * \param[in] soc           SOCKET for communication.
	 * \param[in] sendQueue     Link to queue for sending.
	 * \param[in] receiveQueue  Link to queue for receiving.
	 */
	ConnectionTCP(SOCKET soc, MyQueue<T>* sendQueue, MyQueue<T>* receiveQueue);

	/**
	 * \brief Method for sending front coordinate from sending queue to client.
	 */
	void sendCoord();

	/**
	 * \brief   Function for receiving coordinate from client and adding to recvQueue.
	 * \return  0 if coordinate was received,
	 *          1 if SOCKET haven't receive any pocket,
	 *          -15 if was winSock error.
	 */
	int recvCoord();
};

#include "tcpConnection.inl"

#endif // TCP_CONNECTION_H
