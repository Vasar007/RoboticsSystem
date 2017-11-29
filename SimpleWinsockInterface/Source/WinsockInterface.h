#ifndef WINSOCK_INTERFACE_H
#define WINSOCK_INTERFACE_H

#include <unordered_map>
#include <memory>
#include <winsock2.h>
#include <Ws2tcpip.h>

#include "Utilities.h"

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


/**
 * \brief My working namespace (Vasily Vasilyev).
 */
namespace vasily
{
	
/**
 * \brief	Standard class which provides an interface to interact with WinSock.
 * \details You should create derrived class from this, implement three main functions 
 *			(run, waitLoop, launch) and then
 *			call all functional stuff that you needed. You should work as presented below:
 * \code
 * int main()
 * {
 *	const int SERVER_PORT	= 9997;
 *	const std::string SERVER_IP = "192.168.0.100";
 *  
 *	Client client(SERVER_PORT, SERVER_IP);
 *  
 *	client.init();
 *	client.launch();
 *	client.run();
 *	
 *	return 0;
 * }
 * \endcode
 */
class WinsockInterface
{
public:
	/**
	 * \brief List of non-socket errors type that can occured.
	 */
	enum class ErrorType
	{
		FAILED_INITIALIZE_WSDATA	= -1,
		FAILED_CREATE_SOCKET		= -2,
		FAILED_BIND					= -3,
		FAILED_ACCEPT				= -4,
		FAILED_LISTEN				= -5,
		FAILED_SELECT_CALL			= -6,
		FAILED_ACCEPT_NEW_CLIENT	= -7
	};


protected:
	/**
	 * \brief Structure contains information about the Windows Sockets implementation.
	 */
	WSADATA							_wsaData;

	/**
	 * \brief Socket used to send coordinates.
	 */
	SOCKET							_socketSend;

	/**
	 * \brief Socket used to receive coordinates.
	 */
	SOCKET							_socketReceive;

	/**
	 * \brief Structure used to keep socket address to send.
	 */
	SOCKADDR_IN						_socketSendAddress;

	/**
	 * \brief Structure used to keep socket address to receive.
	 */
	SOCKADDR_IN						_socketReceiveAddress;

	/**
	 * \brief All information about socket and type of connection.
	 */
	std::unique_ptr<ADDRINFO>		_addressInfo;

	/**
	 * \brief Flag used to show status of network interaction.
	 */
	bool							_isRunning;

	/**
	 * \brief Flag used to check whether winsocket had been initialized.
	 */
	bool							_isInitialized;

	/**
	 * \brief Receive buffer that is used to keep answers from clients.
	 */
	char*							_buffer;

	/**
	 * \brief Buffer that is used to keep clients addresses.
	 */
	char*							_message;

	/**
	 * \brief Size of receive buffer, this is string length.
	 */
	static constexpr std::size_t	_MAXRECV = 1024u;


	/**
	 * \brief Table of WinSock errors, which you can get from function WSAGetLastError().
	 */
	static const std::unordered_map<int, std::string> _TABLE_OF_ERRORS;


	/**
	 * \brief				Function initiates use of the Winsock DLL by a process.
	 * \param[out] wsaData	A reference to the WSADATA data structure that is to receive details
	 *						of the WinSock implementation.
	 */
	void		 initWinsock(WSADATA& wsaData) const;

	/**
	 * \brief					Function bindes a socket that is bound to a specific transport 
	 *							service provider.
	 * \param[out] socketToInit	Descriptor referencing socket.
	 * \param[in] aiProtocol	The protocol to be used. The possible options for the protocol
	 *							parameter are specific to the address family and socket type
	 *							specified. 
	 */
	void		 initSocket(SOCKET& socketToInit, const int aiProtocol = 6) const;
				 
	/**
	 * \brief						Function associates a local address with a socket.
	 * \param[in] socketToBind		A descriptor identifying an unbound socket.
	 * \param[out] socketAddress	A pointer to a sockaddr structure of the local address to 
	 *								assign to the bound socket.
	 * \param port					Port for the socket that is being bound.
	 */
	void		 bindSocket(const SOCKET& socketToBind, SOCKADDR_IN& socketAddress, 
							const int port) const;

	/**
	 * \brief					Function places a socket in a state in which it is listening for
	 *							an incoming connection.
	 * \param[in] socketToList	A descriptor identifying a bound, unconnected socket.
	 * \param[in] backlog		The maximum length of the queue of pending connections.
	 */
	void		 listenOn(const SOCKET& socketToList, const int backlog = 10) const;

	/**
	 * \brief						Function establishes a connection to a specified socket.
	 * \param[in] port				Port for connection.
	 * \param[in] ip				IP address for connection.
	 * \param[in] socketToConnect	A descriptor identifying an unconnected socket.
	 * \param[out] socketAddress	A pointer to the sockaddr structure to which the connection
	 *								should be established.
	 * \return						If no error occurs, connect returns zero. Otherwise, it 
	 *								returns SOCKET_ERROR, and a specific error code can be 
	 *								retrieved by calling WSAGetLastError.
	 */
	bool		 tryConnect(const int port, const std::string& ip, 
							const SOCKET& socketToConnect, SOCKADDR_IN& socketAddress) const;
	
	/**
	 * \brief				Function sends data on a connected socket.
	 * \param[out] socketToSend	A descriptor identifying a connected socket.
	 * \param[out] data			A buffer containing the data to be transmitted.
	 */
	void		 sendData(const SOCKET& socketToSend, const std::string& data) const;

	/**
	 * \brief						Function sets timeout for socket.
	 * \param[in] socketForSetting	A descriptor identifying a socket.
	 * \param[in] seconds			Time interval, in seconds.
	 * \param[in] microseconds		Time interval, in microseconds.
	 */
	void		setTimeout(const SOCKET& socketForSetting,
							const long seconds, const long microseconds) const;

	/**
	 * \brief Main infinite working loop. All network logic should be placed here.
	 */
	virtual void waitLoop() = 0;


public:
	/**
	  * \brief Default constructor that initializes protected fields (except sockets).
	  */
						WinsockInterface();

	/**
	 * \brief Default destructor (delete buffers and close all sockets and WinSock data).
	 */
	virtual				~WinsockInterface() noexcept;

	/**
	 * \brief			Deleted copy constructor.
	 * \param[in] other Other object.
	 */
						WinsockInterface(const WinsockInterface& other)		= delete;

	/**
	 * \brief			Deleted copy assignment operator.
	 * \param[in] other Other object.
	 * \return			Returns nothing because it's deleted.
	 */
	WinsockInterface&	operator=(const WinsockInterface& other)			= delete;

	/**
	 * \brief				Deleted move constructor.
	 * \param[out] other	Other object.
	 */
						WinsockInterface(WinsockInterface&& other) noexcept = delete;

	/**
	 * \brief				Deleted move assignment operator.
	 * \param[out] other	Other object.
	 * \return				Returns nothing because it's deleted.
	 */
	WinsockInterface&	operator=(WinsockInterface&& other) noexcept		= delete;

	/**
	 * \brief	Displaying cuurent network interactions.
	 * \return	True if interface running, false otherwise.
	 */
	bool				isRun() const;

	/**
	 * \brief Fuction initializes WSDATA and sockets.
	 */
	void				init();

	/**
	 * \brief Main method which starts infinite working loop.
	 */
	virtual void		run() = 0;

	/**
	 * \brief Fuction processes sockets.
	 */
	virtual void		launch() = 0;
};

}

#endif // WINSOCK_INTERFACE_H
