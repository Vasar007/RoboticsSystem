#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <WinSock2.h>

#include "MyThread.hpp"
#include "MyQueue.hpp"
#include "tcpConnection.hpp"

/**
 * \brief       Class implements server for client connections.
 * \tparam T    Type of cordinates for communication.
 */
template<typename T>
class ServerTCP
{
private:

	/**
	 * \brief Main SOCKET for binding.
	 */
	SOCKET _sockMain = INVALID_SOCKET;

	/**
	 * \brief Class for processing client connection.
	 */
	ConnectionTCP<T>* _curConnectedClient = nullptr;
	
	/**
	 * \brief Parallel thread for receiving new cordinates from client.
	 */
	MyThread _coordsInputStream;
	
	/**
	 * \brief Parallel thread for sending cordinates to client.
	 */
	MyThread _coordsOuputStream;
	
	/**
	 * \brief Parallel thread for supporting one connection.
	 */
	MyThread _supportOneConnection;
	
	/**
	 * \brief Parallel thread for accepting new clients.
	 */
	MyThread _parallelAccept;

	/**
	 * \brief Queue of SOCKET for communication with clients.
	 */
	MyQueue<SOCKET> _socketConnectionsQueue;

	/**
	 * \brief Time of previous point acceptance.
	 */
	int _prevRecieveTime;
	
	/**
	 * \brief Time for waiting before the disconnection.
	 */
	int _timeOut;

	/**
	 * \brief Flag if we are connected.
	 */
	bool _connected = false;

    /**
	 * \brief Field describing current status of server.
	 */
	myInterface::StaticField<std::string> _statusField;

	/**
	 * \brief           Method for initializing listening SOCKET.
	 * \param[in] port  Port for listening.
	 * \return          0 if no errors, else code of error.
	 */
	int initialise(int port);

	/**
	 * \brief           Static method for parallel receiving coordinates.
	 * \param[in] mt    Mutex for locking thread.
	 * \param[in] f     Flag for closing thread.
	 * \param[in] ins   Instance of this class.
	 */
	static void recieveThread(std::mutex* mt, bool* f, ServerTCP* ins);

	/**
	 * \brief           Static method for parallel sending coordinates.
	 * \param[in] mt    Mutex for locking thread.
	 * \param[in] f     Flag for closing thread.
	 * \param[in] ins   Instance of this class.
	 */
	static void sendThread(std::mutex* mt, bool* f, ServerTCP* ins);

	/**
	 * \brief                   Static method for supporting one actual connection.
	 * \param[in] mt            Mutex for locking thread.
	 * \param[in] f             Flag for closing thread.
	 * \param[in] instance      Instance of this class.
	 * \param[in] sendQueue     Pointer to Queue of sending coordinates.
	 * \param[in] recieveQueue  Pointer to queue of receiving coordinates.
	 */
	static void oneConnectionSupportThread(std::mutex* mt, bool* f, ServerTCP* instance,
                                           MyQueue<T>* sendQueue, MyQueue<T>* recieveQueue);

	/**
	 * \brief               Satic method for parallel accepting new clients.
	 * \param[in] mt        Mutex for locking thread.
	 * \param[in] f         Flag for closing thread.
	 * \param[in] instance  Instance of this class.
	 */
	static void paralelAcceptThread(std::mutex* mt, bool* f, ServerTCP* instance);

public:

	/**
	 * \brief               Constructor
	 * \param[in] port      Port for accepting new clients.
	 * \param[in] timeOut   Time for checking new coordinates.
	 */
	explicit ServerTCP(int port, int timeOut);

	/**
	 * \brief                   Method for trying to accept any connection.
	 * \param[in] sendQueue     Link to queue of sending coordinates.
	 * \param[in] recieveQueue  Link to queue of receiving coordinates.
	 * \return                  0 if no errors, -1 if no connections.
	 */
	int tryAccept(MyQueue<T>* sendQueue, MyQueue<T>* recieveQueue);

	/**
	 * \brief                   Method for accepting new client.
	 * \param[in] sendQueue     Link to queue of sending coordinates.
	 * \param[in] recieveQueue  Link to queue of reciving coordinates.
	 * \return                  0 if no errors, else non-zero.
	 */
	int forceAccept(MyQueue<T>* sendQueue, MyQueue<T>* recieveQueue);

	/**
	 * \brief                   Method for starting working in one client mode.
	 * \param[in] sendQueue     Link to queue of sending coordinates.
	 * \param[in] recieveQueue  Link to queue of reciving coordinates.
	 */
	void supportOneConnection(MyQueue<T>* sendQueue, MyQueue<T>* recieveQueue);

	/**
	 * \brief Method for finishing working.
	 */
	void stopServer();

	/**
	 * \brief Default destructor.
	 */
	~ServerTCP();
};

#include "tcpServer.inl"

#endif // TCP_SERVER_H
