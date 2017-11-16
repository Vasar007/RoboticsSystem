#ifndef ROBOT_CONNECT
#define ROBOT_CONNECT
#pragma once

#define WIN32_LEAN_AND_MEAN

#include <string>
#include <winsock2.h>

#include "MyQueue.hpp"
#include "MyThread.hpp"
#include "RobotSend.hpp"
#include "RobotRecieve.hpp"
#include "StaticField.hpp"

/**
 * \brief Goddy class of comunication with robot.
 * \tparam T Type of points for communication.
 */
template<typename T>
class RobotConnect
{
	/**
	 * \brief Port for sending.
	 */
	int _ports;
	
	/**
	 * \brief Port for reciving.
	 */
	int _portr; 
	
	/**
	 * \brief IP adress of server.
	 */
	std::string _serverAddrString;	
	/**
	 * \brief Time delay between points processing.
	 */
	int _segtime;
	
	/**
	 * \brief Robot speed for calculations.
	 */
	double _robotSpeed;
	
	/**
	 * \brief Type of cordinates in which we move.
	 */
	int _syscoord;
	// 0 - JOINT | 2 - WORLD
	
	/**
	 * \brief Time delay between checking connectoin.
	 */
	int _disconnectTime1;
	
	/**
	 * \brief Time between last reciving and starting restart connection.
	 */
	int _disconnectTime2;

	/**
	 * \brief SOCKET for sending points. 
	 */
	SOCKET _sockSend;
	
	/**
	 * \brief SOCKET for reciving points.
	 */
	SOCKET _sockRecv;

	/**
	 * \brief Pointer to queue with points for sending.
	 */
	MyQueue<T>* _sendingQueue;
	
	/**
	 * \brief Pointer to queue with points which recived.
	 */
	MyQueue<T>* _recivingQueue;

	/**
	 * \brief Queue of points which was send but there processing wasn't confirmed.
	 */
	MyQueue<T> _cloneQueue;

	/**
	 * \brief Class for sending points.
	 */
	RobotSend<T> *_robotSend;

	/**
	 * \brief Class for reciving points.
	 */
	RobotRecieve<T> *_robotRecieve;

	/**
	 * \brief Thread for sending points.
	 */
	MyThread _threadSend;

	/**
	 * \brief Thread for reciving points.
	 */
	MyThread _threadRecv;

	/**
	 * \brief Thread for proced comunication with robot.
	 */
	MyThread _mainThread;

	/**
	 * \brief Flag if connected to robot now.
	 */
	bool _connected;

	myInterface::StaticField<std::string> _connectionField;

	/**
	 * \brief Method for making one connection and closing it.
	 * \return 0 if no errors, else non-zero.
	 */
	int healServerSecondConnection() const;

	/**
	 * \brief Function for making connection to the robot.
	 * \return 0 if no error, else non-zero.
	 */
	int conRobot();

	/**
	 * \brief Static function for paralel reciving points.
	 * \param mt Mutex for locking thread.
	 * \param f Flag for finishing thread.
	 * \param ins Instace of this class.
	 */
	static void reverseStream(std::mutex* mt, bool* f, RobotConnect* ins);

	/**
	 * \brief Static function for paralel sending points
	 * \param mt Mutex for locking thread.
	 * \param f Flag for finishing thread.
	 * \param ins Instance of this class.
	 */
	static void sendParalel(std::mutex* mt, bool* f, RobotConnect* ins);

	/**
	 * \brief Function for starting communication with robot.
	 * \return 0 if no errors, else non-zero.
	 */
	int startWorking();

	/**
	 * \brief Function for begining communication with robot.
	 */
	void tryConnect();

	/**
	 * \brief Functon for ending communication wuth robot.
	 */
	void finishWorking();

	/**
	 * \brief Function for resarting communication with robot.
	 */
	void restart();

	/**
	 * \brief Static function for paralel stable comunication with robot.
	 * \param mt Mutex for locking thread.
	 * \param f Flag for finishing thead.
	 * \param ins Instance of this class.
	 */
	static void mainLoop(std::mutex* mt, bool* f, RobotConnect* ins);

public:

	/**
	 * \brief Flag for restarting communication.
	 */
	bool _forcedRestart;

	/**
	 * \brief Constructor.
	 * \param configFileName File name of file with params for connection to robot. 
	 * \param sendingQueue Pointer to queue of points for sending.
	 * \param recivingQueue Pointer to queue of points for reciving.
	 */
	RobotConnect(std::string configFileName, MyQueue<T>* sendingQueue, MyQueue<T>* recivingQueue);

	/**
	 * \brief Method for starting comunication with robot.
	 */
	void startMainLoop();

	/**
	 * \brief Method for finishing communication with robot.
	 */
	void stopMainLoop();

	/**
	 * \brief Method for force restarting communication with robot.
	 */
	void restartMainLoop();

	/**
	 * \brief Default destructor.
	 */
	~RobotConnect();
};

#include "RobotComunicationDifinition.inl"

#endif // ROBOT_CONNECT
