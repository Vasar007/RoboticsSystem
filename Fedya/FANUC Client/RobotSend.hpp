#ifndef ROBOT_SEND
#define ROBOT_SEND
#pragma once

#include <winsock2.h>

#include "MyQueue.hpp"

/**
 * \brief Class for supporting sending to robot.
 * \tparam T Type of cordinates.
 */
template<typename T>
class RobotSend
{
	/**
	 * \brief SOCKET from what we send to robot.
	 */
	SOCKET _sockSend;
	
	/**
	 * \brief Pointer to MyQueue where stored points which is on processig on robot.
	 */
	MyQueue<T>* _cloneQueue;
	
	/**
	 * \brief Distance in 6 dimsional space which means if diference betwin two points is greater tha it so we should send next point two times for bieng sure we was in this positin.
	 */
	int _acyrancy;

	/**
	 * \brief Method for sending point and adding it to cloneQueue.
	 * \param rc point which we send.
	 * \return 0 if no error, else non-zero.
	 */
	int sendCoord(T rc);

public:
	/**
	 * \brief Flag if was first point sent.
	 */
	bool _wasFirstPoint;
	
	/**
	 * \brief Previous point which we send.
	 */
	T _prevCoord;

	/**
	 * \brief Constructor.
	 * \param soc SOCKET from which we send points.
	 * \param cloneQueue Pointer to queue with points which in process.
	 * \param acyrancy Distance in 6 dimsional space which means if diference betwin two points is greater tha it so we should send next point two times for bieng sure we was in this positin.
	 */
	RobotSend(SOCKET soc, MyQueue<T>* cloneQueue, int acyrancy = 1000);

	/**
	 * \brief Function for sending point to robot.
	 * \param rc Point which should be send to robot.
	 * \param forceSend Flag if this point should be send in any case.
	 * \return 0 if point was send, else 1.
	 */
	int sendRobotCoord(T rc, bool forceSend = false);

	/**
	 * \brief Function for sending previous point to robot.
	 * \return 0 if point was send, else 1.
	 */
	int sendPrevCoord();

	/**
	 * \brief Function for sending points in cloneQueue(which was sended to robot but wasn't confermed there's complit)
	 * \return 0 if points was send.
	 */
	int resendCloneQueue();
};

#include "RobotSendDifinition.hpp"

#endif
