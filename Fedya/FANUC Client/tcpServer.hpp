#ifndef TCP_SERVER
#define TCP_SERVER
#pragma once

#include <WinSock2.h>
#include "MyThread.hpp"
#include "MyQueue.hpp"
#include "tcpConnection.hpp"

/**
 * \brief Class impliments server for client connections.
 * \tparam T Type of cordinates for comunication.
 */
template<typename T>
class ServerTCP
{
	/**
	 * \brief Main SOCKET for binding.
	 */
	SOCKET _main;

	/**
	 * \brief Class for processing client connection.
	 */
	ConnectionTCP<T>* _contcp;
	
	/**
	 * \brief Paralel thread for reciving new points from client.
	 */
	MyThread _conSocketWorkin;
	
	/**
	 * \brief Paralel thread for sending points to client.
	 */
	MyThread _conSocketWorkout;
	
	/**
	 * \brief Paralel thread for support one connection.
	 */
	MyThread _oneConnection;
	
	/**
	 * \brief Paralel thread for accepting new clients.
	 */
	MyThread _paralelAccept;

	/**
	 * \brief Queue of SOCKET for communication with clients.
	 */
	MyQueue<SOCKET> _socketConnectionsQueue;

	/**
	 * \brief time of previous point acception.
	 */
	int _prevRecieve;
	
	/**
	 * \brief Time for waiting before disconnect.
	 */
	int _timeOut;

	/**
	 * \brief Flag if we are connected.
	 */
	bool _connected = false;

	/**
	 * \brief Method for intialsing listening SOCKET.
	 * \param port Port for listening.
	 * \return 0 if no errors, else code of error.
	 */
	int initialise(int port);

	/**
	 * \brief Static function for paralel reciving points.
	 * \param mt Mutex for locking thread.
	 * \param f Flag for ending thread.
	 * \param ins Instance of this class.
	 */
	static void tcpWorkin(std::mutex* mt, bool* f, ServerTCP* ins);

	/**
	 * \brief Static function for paralel sending points
	 * \param mt Mutex for locking thread.
	 * \param f Flag for ending thread.
	 * \param ins Instance of this class.
	 */
	static void tcpWorkout(std::mutex* mt, bool* f, ServerTCP* ins);

	/**
	 * \brief Static function for supporting oneactual connection.
	 * \param mt Mutex for locking thread.
	 * \param f Flag for ending thread.
	 * \param instance Instance of this class.
	 * \param sendQueue Pointer to Queue of sending points.
	 * \param recieveQueue Pointer to queue of reciving points.
	 */
	static void oneConnection(std::mutex* mt, bool* f, ServerTCP* instance, MyQueue<T>* sendQueue,
	                          MyQueue<T>* recieveQueue);

	/**
	 * \brief Satic function for paralel accepting new clients.
	 * \param mt Mutex for locking thread.
	 * \param f Flag for ending thread.
	 * \param instance Instance of this class.
	 */
	static void paralelAccept(std::mutex* mt, bool* f, ServerTCP* instance);

public:

	/**
	 * \brief Constructor:(
	 * \param port Port for finding new clients.
	 * \param timeOut Time for checking new points.
	 */
	explicit ServerTCP(int port, int timeOut);

	/**
	 * \brief Method for trying to accept any connection.
	 * \param sendQueue Pointer to queue of sending points.
	 * \param recieveQueue Pointer to queue of reciving queue.
	 * \return 0 if no errors, -1 if no connections.
	 */
	int tryAccept(MyQueue<T>* sendQueue, MyQueue<T>* recieveQueue);

	/**
	 * \brief Method for accepting new client.
	 * \param sendQueue Pointer to queue of sending points.
	 * \param recieveQueue Pointer to queue of reciving points.
	 * \return 0 if no errors, else non-zero.
	 */
	int forceAccept(MyQueue<T>* sendQueue, MyQueue<T>* recieveQueue);

	/**
	 * \brief Method for starting working in one client mode.
	 * \param sendQueue Pointer to queue of sending points.
	 * \param recieveQueue Pointer to queue of reciving points.
	 */
	void supportOneConnection(MyQueue<T>* sendQueue, MyQueue<T>* recieveQueue);

	/**
	 * \brief Method for finishing working.
	 */
	void closeServer();

	/**
	 * \brief Default destructor.
	 */
	~ServerTCP();
};

#include "tcpServerDifinition.hpp"

#endif
