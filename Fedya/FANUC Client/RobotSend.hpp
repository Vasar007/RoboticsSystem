#ifndef ROBOT_SEND_H
#define ROBOT_SEND_H

#include <winsock2.h>

#include "MyQueue.hpp"

/**
 * \brief		Class for supporting sending coordinates to robot.
 * \tparam T	Type of cordinates.
 */
template<typename T>
class RobotSend
{
private:

	/**
	 * \brief SOCKET for sending coordinates to robot.
	 */
	SOCKET _sockSend = INVALID_SOCKET;
		
	/**
	 * \brief Link to MyQueue where stored coordinates which are on processing on robot.
	 */
	MyQueue<T>* _cloneQueue = nullptr;
		
	/**
	 * \brief	Distance in six dimensional space which means:
	 *          if difference between two coordinates is greater than it,
	 *          so we should send next point two times for being sure we were in this position.
	 */
	double _accuracy;
	
	/**
	 * \brief		    Method for sending coordinates and adding it to cloneQueue.
	 * \param[in] rc    Coordinates which we send.
	 * \return		    0 if no error, else non-zero.
	 */
	int sendCoord(const T rc);
	
public:
	/**
	 * \brief Flag if it was first coordinates sent.
	 */
	bool _wasFirstPoint = false;
		
	/**
	 * \brief Previous coordinates which we send.
	 */
	T _previousCoord;
	
	/**
	 * \brief				    Constructor.
	 * \param[in] soc		    SOCKET for sending coordinates to robot.
	 * \param[ni] cloneQueue    Link to MyQueue where stored coordinates,
	 *                          which are on processing on robot.
	 * \param[in] acyrancy	    Distance in six dimensional space which means:
	 *                          if difference between two coordinates is greater than it,
	 *                          so we should send next point two times 
	 *                          for being sure we were in this position.
	 */
	explicit RobotSend(SOCKET soc, MyQueue<T>* cloneQueue, int acyrancy = 1000);
	
	/**
	 * \brief			    Method for sending coordinate to robot.
	 * \param[in] rc		Coordinates which should be sent to robot.
	 * \param[in] forceSend Flag if this coordinate should be sent in any case.
	 * \return			    0 if point was send, else 1.
	 */
	int moveRobot(const T rc,const bool forceSend = false);
	
	/**
	 * \brief	Method for sending previous coordinate to robot.
	 * \return	0 if point was send, else 1.
	 */
	int sendPreviousCoord();
	
	/**
	 * \brief	Method for sending coordinates in cloneQueue
	 *          (Which was sent to robot, but they weren't confirmed there's computing)
	 * \return	0 if points was send.
	 */
	int resendCloneQueue();
};

#include "RobotSend.inl"

#endif // ROBOT_SEND_H
