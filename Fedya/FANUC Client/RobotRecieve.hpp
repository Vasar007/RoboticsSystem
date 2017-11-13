#ifndef ROBOT_RECIEVE
#define ROBOT_RECIEVE
#pragma once


#include <winsock2.h>

#include "MyQueue.hpp"


/**
 * \brief Class for aupprting reciving points from robot.
 * \tparam T Type of points for reciving.
 */
template<typename T>
class RobotRecieve
{
	/**
	 * \brief SOCKET for accesing points.
	 */
	SOCKET _sockRecv;
	
	/**
	 * \brief Queue of points which proceed on robot.
	 */
	MyQueue<T>* _cloneQueue;
	
	/**
	 * \brief Queue of points which was recived from robot.
	 */
	MyQueue<T>* _returnQueue;
	
	/**
	 * \brief String which was recived from robot.
	 */
	std::string _sbuf;
	
	/**
	 * \brief Max time delay between reciving points.
	 */
	int _readTime;
public:

	/**
	 * \brief Previous recived point.
	 */
	T _prevCoord;

	/**
	 * \brief Method for recveving and parsing points from robot.
	 * \return 0 if smth was recived, else non-zero.
	 */
	int readCoord();

	/**
	 * \brief Constructor
	 * \param socRecv SOCKET for reciving.
	 * \param cloneQueue Poiner to queue of points which was send.
	 * \param returnQueue Pointer to queue of points which recived.
	 * \param readTime Max time delay between reciving points.
	 */
	RobotRecieve(SOCKET socRecv, MyQueue<T>* cloneQueue, MyQueue<T>* returnQueue, int readTime);
};

#include "RobotRecieveDifinition.hpp"

#endif