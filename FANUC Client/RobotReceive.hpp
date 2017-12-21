#ifndef ROBOT_RECEIEVE_H
#define ROBOT_RECEIEVE_H

#include <winsock2.h>

#include "MyQueue.hpp"

/**
 * \brief       Class for supporting receiving coordinates from robot.
 * \tparam T    Type of coordinates for receiving.
 */
template<typename T>
class RobotReceieve
{
private:

	/**
	 * \brief SOCKET for accessing coordinates.
	 */
	SOCKET _sockRecv = INVALID_SOCKET;
	
	/**
	 * \brief Queue of coordinates which proceed on robot.
	 */
	MyQueue<T>* _cloneQueue = nullptr;
	
	/**
	 * \brief Queue of coordinates which was received from robot.
	 */
	MyQueue<T>* _returnQueue = nullptr;
	
	/**
	 * \brief String which was received from robot.
	 */
	std::string _stringBuffer = "";
	
	/**
	 * \brief Max time delay between receiving coordinates.
	 */
	int _receieveTime = 100;
public:

	/**
	 * \brief Previous received coordinates.
	 */
	T _prevCoord;

	/**
	 * \brief   Method for receiving and parsing coordinates from robot.
	 * \return  0 if smth was received, else non-zero.
	 */
	int receiveCoord();

	/**
	 * \brief                   Constructor
	 * \param[in] socRecv       SOCKET for receiving.
	 * \param[in] cloneQueue    Link to queue of coordinates which was sent.
	 * \param[in] returnQueue   Link to queue of coordinates which was received.
	 * \param[in] readTime      Max time delay between receiving coordinates.
	 */
	explicit RobotReceieve(SOCKET socRecv, MyQueue<T>* cloneQueue,
        MyQueue<T>* returnQueue, int readTime);
};

#include "RobotReceive.inl"

#endif // ROBOT_RECEIEVE_H