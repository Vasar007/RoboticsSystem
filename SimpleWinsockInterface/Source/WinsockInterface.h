#ifndef WINSOCK_INTERFACE_H
#define WINSOCK_INTERFACE_H
#pragma once

#include <map>
#include <vector>
#include <winsock2.h>
#include <Ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


/**
 * \brief	Standard class which provides an interface to interact with WinSock.
 * \details You should create derrived class from this, implement two main functions and then
 *			call all functional stuff that you needed.
 */
class WinsockInterface
{
	public:
		/**
		 * \brief List of non-socket errors type that can occured.
		 */
		enum ErrorType
		{
			FAILED_INITIALIZE_WSDATA = -1,
			FAILED_CREATE_SOCKET	 = -2,
			FAILED_BIND				 = -3,
			FAILED_ACCEPT			 = -4,
			FAILED_LISTEN			 = -5,
			FAILED_SELECT_CALL		 = -6,
			FAILED_ACCEPT_NEW_CLIENT = -7
		};
	
	
	protected:
		/**
		 * \brief Structure contains information about the Windows Sockets implementation.
		 */
		WSADATA			 _wsaData;

		/**
		 * \brief Socket for sending coordinates.
		 */
		SOCKET			 _socketSend;

		/**
		 * \brief Socket for reciving coordinates.
		 */
		SOCKET			 _socketRecive;
	
		/**
		 * \brief Structure for keeping sending socket address.
		 */
		SOCKADDR_IN		 _socketSendAddress;

		/**
		 * \brief Structure for keeping reciving socket address.
		 */
		SOCKADDR_IN		 _socketReciveAddress;

		/**
		 * \brief All information about socket and type of connection.
		 */
		ADDRINFO*		 _addressInfo;

		/**
		 * \brief Boolean flag for status displaying of network interaction.
		 */
		bool			 _isRunning;
	
		/**
		 * \brief Receive buffer for keeping answers from clients.
		 */
		char*			 _buffer;

		/**
		 * \brief Buffer for keeping addresses from clients.
		 */
		char*			 _message;

		/**
		 * \brief Size of receive buffer, this is string length.
		 */
		const int		 _MAXRECV = 1024;
	
		/**
		 * \brief Table of WinSock errors, which you can get from function WSAGetLastError().
		 */
		const std::map<int, std::string> _TABLE_OF_ERRORS =
		{
			{ 6,	 "Specified event object handle is invalid." },
			{ 8,	 "Insufficient memory available." },
			{ 87,	 "One or more parameters are invalid." },
			{ 995,	 "Overlapped operation aborted." },
			{ 996,	 "Overlapped I/O event object not in signaled state." },
			{ 997,	 "Overlapped operations will complete later." },
			{ 10004, "Interrupted function call." },
			{ 10009, "File handle is not valid." },
			{ 10013, "Permission denied." },
			{ 10014, "Bad address" },
			{ 10022, "Invalid argument." },
			{ 10024, "Too many open files." },
			{ 10035, "Resource temporarily unavailable." },
			{ 10036, "Operation now in progress." },
			{ 10037, "Operation already in progress." },
			{ 10038, "Socket operation on nonsocket." },
			{ 10039, "Destination address required." },
			{ 10040, "Message too long." },
			{ 10041, "Protocol wrong type for socket." },
			{ 10042, "Bad protocol option." },
			{ 10043, "Protocol not supported." },
			{ 10044, "Socket type not supported." },
			{ 10045, "Operation not supported." },
			{ 10046, "Protocol family not supported." },
			{ 10047, "Address family not supported by protocol family." },
			{ 10048, "Address already in use." },
			{ 10049, "Cannot assign requested address." },
			{ 10050, "Network is down." },
			{ 10051, "Network is unreachable." },
			{ 10052, "Network dropped connection on reset." },
			{ 10053, "Software caused connection abort." },
			{ 10054, "Connection reset by peer." },
			{ 10055, "No buffer space available." },
			{ 10056, "Socket is already connected." },
			{ 10057, "Socket is not connected." },
			{ 10058, "Cannot send after socket shutdown." },
			{ 10059, "Too many references." },
			{ 10060, "Connection timed out." },
			{ 10061, "Connection refused." },
			{ 10062, "Cannot translate name." },
			{ 10063, "Name too long." },
			{ 10064, "Host is down." },
			{ 10065, "No route to host." },
			{ 10066, "Directory not empty." },
			{ 10067, "Too many processes." },
			{ 10068, "User quota exceeded." },
			{ 10069, "Disk quota exceeded." },
			{ 10070, "Stale file handle reference." },
			{ 10071, "Item is remote." },
			{ 10091, "Network subsystem is unavailable." },
			{ 10092, "Winsock.dll version out of range." },
			{ 10093, "Successful WSAStartup not yet performed." },
			{ 10101, "Graceful shutdown in progress." },
			{ 10102, "No more results." },
			{ 99999, "Some another error occured..." }
		};

	
		/**
		 * \brief		  Function initiates use of the Winsock DLL by a process.
		 * \param wsaData A reference to the WSADATA data structure that is to receive details of
		 *				  the WinSock implementation.
		 */
		void		 initWinsock(WSADATA& wsaData) const;

		/**
		 * \brief				Function bindes a socket that is bound to a specific transport 
		 *						service provider.
		 * \param socketForInit Descriptor referencing socket.
		 * \param aiProtocol	The protocol to be used. The possible options for the protocol
		 *						parameter are specific to the address family and socket type
		 *						specified. 
		 */
		void		 initSocket(SOCKET& socketForInit, const int aiProtocol = 6) const;
					 
		/**
		 * \brief				Function associates a local address with a socket.
		 * \param socketForBind A descriptor identifying an unbound socket.
		 * \param socketAddress A pointer to a sockaddr structure of the local address to assign to
		 *						the bound socket.
		 * \param port			Port for the socket that is being bound.
		 */
		void		 bindSocket(const SOCKET& socketForBind, SOCKADDR_IN& socketAddress, 
								const int port) const;
	
		/**
		 * \brief				Function places a socket in a state in which it is listening for
		 *						an incoming connection.
		 * \param socketForLst	A descriptor identifying a bound, unconnected socket.
		 * \param backlog		The maximum length of the queue of pending connections.
		 */
		void		 listenOn(const SOCKET& socketForLst, const int backlog = 10) const;

		/**
		 * \brief					Function establishes a connection to a specified socket.
		 * \param port				Port for connection.
		 * \param ip				IP address for connection.
		 * \param socketForConnect	A descriptor identifying an unconnected socket.
		 * \param socketAddress		A pointer to the sockaddr structure to which the connection
		 *							should be established.
		 * \return					If no error occurs, connect returns zero. Otherwise, it returns
		 *							SOCKET_ERROR, and a specific error code can be retrieved by
		 *							calling WSAGetLastError.
		 */
		bool		 tryConnect(const int port, const std::string& ip, 
								const SOCKET& socketForConnect, SOCKADDR_IN& socketAddress) const;
		
		/**
		 * \brief				Function sends data on a connected socket.
		 * \param socketForSend A descriptor identifying a connected socket.
		 * \param data			A buffer containing the data to be transmitted.
		 */
		void		 sendData(const SOCKET& socketForSend, const std::string& data) const;

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
		virtual		 ~WinsockInterface();
	
		/**
		 * \brief  Displaying cuurent network interactions.
		 * \return True if interface running, false otherwise.
		 */
		bool		 isRun() const;

		/**
		 * \brief Main method which starts infinite working loop.
		 */
		virtual void run() = 0;
};

#endif // WINSOCK_INTERFACE_H
