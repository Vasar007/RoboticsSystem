#ifndef WINSOCK_INTERFACE_H
#define WINSOCK_INTERFACE_H

#include <atomic>
#include <memory>
#include <unordered_map>

#include <winsock2.h>
#include <Ws2tcpip.h>

#include "Utilities.h"

#pragma comment (lib, "ws2_32.lib")


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
 *	constexpr int kServerPort = 9997;
 *	constexpr char kServerIP[] = "192.168.0.100";
 *  
 *	Client client(kServerPort, kServerIP);
 *  
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


    /**
      * \brief Default constructor that initializes protected fields (except sockets).
      */
                        WinsockInterface();

    /**
     * \brief Default destructor (close all sockets and WinSock data).
     */
    virtual				~WinsockInterface();

    /**
     * \brief			Deleted copy constructor.
     * \param[in] other Other object.
     */
                        WinsockInterface(const WinsockInterface& other)		= delete;

    /**
     * \brief			Deleted copy assignment operator.
     * \param[in] other Other object.
     * \return			Return nothing because it's deleted.
     */
    WinsockInterface&	operator=(const WinsockInterface& other)			= delete;

    /**
     * \brief			 Deleted move constructor.
     * \param[out] other Other object.
     */
                        WinsockInterface(WinsockInterface&& other) noexcept = delete;

    /**
     * \brief			 Deleted move assignment operator.
     * \param[out] other Other object.
     * \return			 Return nothing because it's deleted.
     */
    WinsockInterface&	operator=(WinsockInterface&& other) noexcept		= delete;

    /**
     * \brief  Displaying cuurent network interactions.
     * \return True if interface running, false otherwise.
     */
    bool				isRun() const;

    /**
     * \brief Initialize WSDATA and sockets.
     */
    void				init();

    /**
     * \brief Close all initialized data.
     */
    void				close();

    /**
     * \brief Main method which starts infinite working loop.
     */
    virtual void		run() = 0;

    /**
     * \brief Process sockets.
     */
    virtual void		launch() = 0;


protected:
    /**
     * \brief Structure contains information about the Windows Sockets implementation.
     */
    WSADATA							_wsaData;

    /**
     * \brief Socket used to send coordinates.
     */
    SOCKET							_sendingSocket;

    /**
     * \brief Socket used to receive coordinates.
     */
    SOCKET							_receivingSocket;

    /**
     * \brief Structure used to keep socket address to send.
     */
    SOCKADDR_IN						_sendingSocketAddress;

    /**
     * \brief Structure used to keep socket address to receive.
     */
    SOCKADDR_IN						_receivingSocketAddress;

    /**
     * \brief All information about socket and type of connection.
     */
    std::unique_ptr<ADDRINFO>		_addressInfo;

    /**
     * \brief Flag used to show status of network interaction.
     */
    std::atomic_bool				_isRunning;

    /**
     * \brief Flag used to check whether winsocket had been initialized.
     */
    bool							_wasInitialized;

    /**
     * \brief Size of receive buffer, this is string length.
     */
    static constexpr int	        _MAXRECV = 1024;

    /**
     * \brief Receive buffer that is used to keep answers from clients.
     */
    char		                    _buffer[_MAXRECV];

    /**
     * \brief Buffer that is used to keep clients addresses.
     */
    char		                    _messageWithIP[_MAXRECV];

    /**
     * \brief Implementation of type-safe output printer.
     */
    printer::Printer&               _printer = printer::Printer::getInstance();

    /**
     * \brief Table of WinSock errors, which you can get from function WSAGetLastError().
     */
    static const std::unordered_map<int, std::string> _TABLE_OF_ERRORS;


    /**
     * \brief			   Initiate use of the Winsock DLL by a process.
     * \param[out] wsaData A reference to the WSADATA data structure that is to receive details
     *					   of the WinSock implementation.
     */
    void			initWinsock(WSADATA& wsaData) const;

    /**
     * \brief					Bind a socket that is bound to a specific transport 
     *							service provider.
     * \param[out] socketToInit	Descriptor referencing socket.
     * \param[in] aiProtocol	The protocol to be used. The possible options for the protocol
     *							parameter are specific to the address family and socket type
     *							specified. 
     */
    void			initSocket(SOCKET& socketToInit, const int aiProtocol = 6) const;

    /**
     * \brief                    Close socket.
     * \param[out] socketToClose Descriptor referencing socket.
     */
    void			closeSocket(SOCKET& socketToClose) const;
                 
    /**
     * \brief					 Associate a local address with a socket.
     * \param[in] socketToBind	 A descriptor identifying an unbound socket.
     * \param[out] socketAddress A pointer to a sockaddr structure of the local address to 
     *							 assign to the bound socket.
     * \param port				 Port for the socket that is being bound.
     */
    void			bindSocket(const SOCKET& socketToBind, SOCKADDR_IN& socketAddress, 
                               const int port) const;

    /**
     * \brief                  Place a socket in a state in which it is listening for
     *                         an incoming connection.
     * \param[in] socketToList A descriptor identifying a bound, unconnected socket.
     * \param[in] backlog      The maximum length of the queue of pending connections.
     */
    void			listenOn(const SOCKET& socketToList, const int backlog = 10) const;

    /**
     * \brief                     Permit an incoming connection attempt on a socket.
     * \param[in] listeningSocket A descriptor that identifies a socket that has been placed in
     *                            a listening state with the listen function. The connection is
     *                            actually made with the socket that is returned by accept.
     * \param[out] messageWithIP  Buffer to write accepted IP address.
     * \return                    If no error occurs, accept returns a value of type SOCKET 
     *                            that is a descriptor for the new socket. This returned value is
     *                            a handle for the socket on which the actual connection is made. 
     *                            Otherwise, a value of INVALID_SOCKET is returned, and a specific
     *                            error code can be retrieved by calling WSAGetLastError.
     */
    SOCKET          acceptSocket(const SOCKET& listeningSocket, char* messageWithIP) const;

    /**
     * \brief					  Establishe a connection to a specified socket.
     * \param[in] port			  Port for connection.
     * \param[in] ip			  IP address for connection.
     * \param[in] socketToConnect A descriptor identifying an unconnected socket.
     * \param[out] socketAddress  A pointer to the sockaddr structure to which the connection
     *							  should be established.
     * \return					  If no error occurs, connect returns zero. Otherwise, it 
     *							  returns SOCKET_ERROR, and a specific error code can be 
     *							  retrieved by calling WSAGetLastError.
     */
    bool			tryConnect(const int port, const std::string& ip, 
                               const SOCKET& socketToConnect, SOCKADDR_IN& socketAddress) const;
    
    /**
     * \brief					    Send data on a connected socket.
     * \param[out] socketForSending	A descriptor identifying a connected socket.
     * \param[out] data			    A buffer containing the data to be transmitted.
     */
    void			sendData(const SOCKET& socketForSending, const std::string& data) const;

    /**
     * \brief                        Receive data from receiving socket.
     * \param[in] socketForReceiving A descriptor identifying a receiving socket.
     * \param[out] messageWithIP     Buffer to write accepted IP address.
     * \param[out] buffer	         Buffer to write received data.
     * \return					     Received data from receiving socket and
     *                               flag to put status of connection.
     */
    std::pair<std::string, bool> receiveData(const SOCKET& socketForReceiving, char* messageWithIP,
                                             char* buffer) const;

    /**
     * \brief					 Set timeout for socket.
     * \param[in] socketToChange A descriptor identifying a socket.
     * \param[in] seconds		 Time interval, in seconds.
     * \param[in] microseconds	 Time interval, in microseconds.
     * \code
     * Example: setTimeout(_sendingSocket, 1000, 0);
     * \endcode
     */
    void			setTimeout(const SOCKET& socketToChange,
                               const long& seconds, const long& microseconds) const;

    /**
     * \brief Main infinite working loop. All network logic should be placed here.
     */
    virtual void	waitLoop() = 0;
};

} // namespace vasily

#endif // WINSOCK_INTERFACE_H
