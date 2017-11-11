#ifndef SOCKET_WORKING
#define SOCKET_WORKING
#pragma once

#include<WinSock2.h>

/**
 * \brief Singlton for working with winSock.
 */
class SocketWorking
{
	/**
	 * \brief Flag if winSock was Initialised.
	 */
	bool _isInitialised;

	/**
	 * \brief Private constructor.
	 */
	SocketWorking();

	/**
	 * \brief Private copy constructor.
	 */
	SocketWorking(const SocketWorking&);

	/**
	 * \brief Private assignment constructor. 
	 * \return Instance of singlton.
	 */
	SocketWorking& operator =(SocketWorking&);
public:

	/**
	 * \brief Static function for getting instance of singlton.
	 * \return Instance of singleton.
	 */
	static SocketWorking& getInstance();

	/**
	 * \brief Method for intialasing winSock.
	 * \return 0 if no error, else non-zero.
	 */
	int initialise();

	/**
	 * \brief Method for de intialise winSock.
	 * \return 0 if no error, else 1
	 */
	int deintialise();

	/**
	 * \brief Method for getting new SOCKET.
	 * \return New SOCKET.
	 */
	SOCKET getFreeSocket();

	/**
	 * \brief Method for deleting SOCKET.
	 * \param soc SOCKET for deleting.
	 */
	void deleteSocket(SOCKET& soc);

	/**
	 * \brief Method for connectong to FANUCM20I-A.
	 * \param serveraddr IP adress of robot.
	 * \param port Port for connecting.
	 * \param disconnectTime2 Time for connecting to robot.
	 * \return SOCKET which connected to robot.
	 */
	SOCKET connectToRobotServer(const char* serveraddr, int port, int disconnectTime2);
};

#include"SocketWorkingDifinition.hpp"

#endif
