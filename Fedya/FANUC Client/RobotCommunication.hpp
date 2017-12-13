#ifndef ROBOT_CONNECT_H
#define ROBOT_CONNECT_H

#include <string>
#include <winsock2.h>

#include "MyQueue.hpp"
#include "MyThread.hpp"
#include "RobotSend.hpp"
#include "RobotReceive.hpp"
#include "StaticField.hpp"

/**
 * \brief       Class of communication with robot.
 * \tparam T    Type of coordinates for communication.
 */
template<typename T>
class RobotConnect
{
private:

	/**
	 * \brief Port for sending.
	 */
	int _sendPort = -1;
	
	/**
	 * \brief Port for receiving.
	 */
	int _recvPort = -1; 
	
	/**
	 * \brief IP address of server.
	 */
	std::string _serverAddressString = "";	
	/**
	 * \brief Time delay between points processing.
	 */
	int _segtime = -1;
	
	/**
	 * \brief Robot speed for calculations.
	 */
	double _robotAssumingSpeed = -1.0;
	
	/**
	 * \brief   Type of coordinates in which we move.
	 * \details type of cordiates: 0 = JOINT | 1 = JOGFRAME |
	 *          2 = WORLDFRAME | 3 = TOOLFRAME | 4 = USER FRAME
	 */
	int _sysCoord = -1;
	
	/**
	 * \brief Time delay between checking connection.
	 */
	int _timeBetweenEchoConnectionChecking = -1;
	
	/**
	 * \brief Time between last receiving and starting restart connection.
	 */
	int _timeBetweenReconnection = -1;

	/**
	 * \brief SOCKET for sending coordinates. 
	 */
	SOCKET _sockSend = INVALID_SOCKET;
	
	/**
	 * \brief SOCKET for receiving coordinates.
	 */
	SOCKET _sockRecv = INVALID_SOCKET;

	/**
	 * \brief Link to queue with points for sending.
	 */
	MyQueue<T>* _sendingQueue = nullptr;
	
	/**
	 * \brief Link to queue with points which received.
	 */
	MyQueue<T>* _receivingQueue = nullptr;

	/**
	 * \brief Queue of coordinates which were sent,
	 *        but there processing wasn't confirmed.
	 */
	MyQueue<T> _cloneQueue;

	/**
	 * \brief Class for sending coordinates.
	 */
	RobotSend<T> *_robotSend = nullptr;

	/**
	 * \brief Class for receiving coordinates.
	 */
	RobotReceieve<T> *_robotReceive = nullptr;

	/**
	 * \brief Thread for sending coordinates.
	 */
	MyThread _threadSend;

	/**
	 * \brief Thread for receiving coordinates.
	 */
	MyThread _threadRecv;

	/**
	 * \brief Thread for proceeding communication with robot.
	 */
	MyThread _mainThread;

	/**
	 * \brief Flag if connected to robot now.
	 */
	bool _connected = false;

	/**
	 * \brief Status field.
	 */
	myInterface::StaticField<std::string> _statusField;

	/**
	 * \brief   Method for making one connection to receiving port and closing it.
	 * \return  0 if no errors, 1 if server error, but it is fixed, else winsock error.
	 */
	int launchReceivingSocketAndClosingIt() const;

	/**
	 * \brief           Static method for parallel receiving coordinates.
	 * \param[in] mt    Mutex for locking thread.
	 * \param[in] f     Flag for closing thread.
	 * \param[in] ins   Instance of this class.
	 */
	static void receivingStream(std::mutex* mt, bool* f, RobotConnect* ins);

	/**
	 * \brief           Static method for parallel sending coordinates
	 * \param[in] mt    Mutex for locking thread.
	 * \param[in] f     Flag for closing thread.
	 * \param[in] ins   Instance of this class.
	 */
	static void sendingStream(std::mutex* mt, bool* f, RobotConnect* ins);

	/**
	 * \brief   Method for launching communication with robot.
	 * \return  0 if no errors, else non-zero.
	 */
	int launchConnection();

	/**
	 * \brief Method for beginning communication with robot.
	 */
	void tryConnect();

	/**
	 * \brief Functon for ending communication with robot.
	 */
	void endConnection();

	/**
	 * \brief Function for restarting communication with robot.
	 */
	void restartConnection();

	/**
	 * \brief           Static method for parallel stable communication with robot.
	 * \param[in] mt    Mutex for locking thread.
	 * \param[in] f     Flag for closing thead.
	 * \param[in] ins   Instance of this class.
	 */
	static void mainLoop(std::mutex* mt, bool* f, RobotConnect* ins);

public:

	/**
	 * \brief Flag for restarting communication.
	 */
	bool _forcedRestart = false;

	/**
	 * \brief                       Constructor.
	 * \param[in] configFileName    Name of file with parameters for connection to robot. 
	 * \param[in] sendingQueue      Link to queue of coordinates for sending.
	 * \param[in] recivingQueue     Link to queue of coordinates for reciving.
	 */
	explicit RobotConnect(const std::string& configFileName, MyQueue<T>* sendingQueue,
        MyQueue<T>* recivingQueue);

	/**
	 * \brief Method for starting communication with robot.
	 */
	void startMainLoop();

	/**
	 * \brief Method for closing communication with robot.
	 */
	void stopMainLoop();

	/**
	 * \brief Method for force-restarting communication with robot.
	 */
	void restartMainLoop();

	/**
	 * \brief Default destructor.
	 */
	~RobotConnect();
};

#include "RobotCommunication.inl"

#endif // ROBOT_CONNECT_H
