#ifndef SOCKET_WORKING_H
#define SOCKET_WORKING_H

#include<WinSock2.h>

/**
 * \brief Singlton for working with winSock.
 */
class SocketWorking
{
private:

	/**
	 * \brief Flag if winSock was initialized.
	 */
	bool _isInitialized;

	/**
	 * \brief Private constructor.
	 */
    SocketWorking();

     /**
     * \brief Deleted constructor.
     */
    SocketWorking(const SocketWorking&) = delete;

    /**
    * \brief Deleted constructor.
    * \return
    */
    SocketWorking& operator =(SocketWorking&) = delete;
    
	/**
	 * \brief Deleted constructor.
	 */
	SocketWorking(const SocketWorking&&) = delete;

	/**
	 * \brief Deleted constructor. 
	 * \return 
	 */
	SocketWorking& operator =(SocketWorking&&) = delete;
public:

	/**
	 * \brief   Static method for getting instance of singlton.
	 * \return  Instance of singlton.
	 */
	static SocketWorking& getInstance();

	/**
	 * \brief   Method for intialazing winSock.
	 * \return  0 if no error, else non-zero.
	 */
	int launchWinSock();

	/**
	 * \brief   Method for shutting down winSock.
	 * \return  0 if no error, else 1
	 */
	int closeWinSock();

	/**
	 * \brief   Method for getting new SOCKET.
	 * \return  New SOCKET.
	 */
	SOCKET getFreeSocket();

	/**
	 * \brief           Method for deleting SOCKET.
	 * \param[in] soc   SOCKET for deleting.
	 */
	void deleteSocket(SOCKET& soc);

	/**
	 * \brief                       Method for connecting to FANUCM20I-A.
	 * \param[in] serveraddr        IP adress of robot.
	 * \param[in] port              Port for connecting.
	 * \param[in] disconnectTime2   Time for connecting to robot.
	 * \return                      SOCKET which connected to robot.
	 */
	SOCKET getConnectedSocket(const char* serveraddr, int port, int disconnectTime2);
};

#endif // SOCKET_WORKING_H
