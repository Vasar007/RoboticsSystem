#include <cassert>
#include <thread>

#include "Utilities.h"
#include "WinsockInterface.h"


namespace vasily
{
	
const std::unordered_map<int, std::string> WinsockInterface::_TABLE_OF_ERRORS = 
{
	{ 6,		"Specified event object handle is invalid." },
	{ 8,		"Insufficient memory available." },
	{ 87,		"One or more parameters are invalid." },
	{ 995,		"Overlapped operation aborted." },
	{ 996,		"Overlapped I/O event object not in signaled state." },
	{ 997,		"Overlapped operations will complete later." },
	{ 10004,	"Interrupted function call." },
	{ 10009,	"File handle is not valid." },
	{ 10013,	"Permission denied." },
	{ 10014,	"Bad address" },
	{ 10022,	"Invalid argument." },
	{ 10024,	"Too many open files." },
	{ 10035,	"Resource temporarily unavailable." },
	{ 10036,	"Operation now in progress." },
	{ 10037,	"Operation already in progress." },
	{ 10038,	"Socket operation on nonsocket." },
	{ 10039,	"Destination address required." },
	{ 10040,	"Message too long." },
	{ 10041,	"Protocol wrong type for socket." },
	{ 10042,	"Bad protocol option." },
	{ 10043,	"Protocol not supported." },
	{ 10044,	"Socket type not supported." },
	{ 10045,	"Operation not supported." },
	{ 10046,	"Protocol family not supported." },
	{ 10047,	"Address family not supported by protocol family." },
	{ 10048,	"Address already in use." },
	{ 10049,	"Cannot assign requested address." },
	{ 10050,	"Network is down." },
	{ 10051,	"Network is unreachable." },
	{ 10052,	"Network dropped connection on reset." },
	{ 10053,	"Software caused connection abort." },
	{ 10054,	"Connection reset by peer." },
	{ 10055,	"No buffer space available." },
	{ 10056,	"Socket is already connected." },
	{ 10057,	"Socket is not connected." },
	{ 10058,	"Cannot send after socket shutdown." },
	{ 10059,	"Too many references." },
	{ 10060,	"Connection timed out." },
	{ 10061,	"Connection refused." },
	{ 10062,	"Cannot translate name." },
	{ 10063,	"Name too long." },
	{ 10064,	"Host is down." },
	{ 10065,	"No route to host." },
	{ 10066,	"Directory not empty." },
	{ 10067,	"Too many processes." },
	{ 10068,	"User quota exceeded." },
	{ 10069,	"Disk quota exceeded." },
	{ 10070,	"Stale file handle reference." },
	{ 10071,	"Item is remote." },
	{ 10091,	"Network subsystem is unavailable." },
	{ 10092,	"Winsock.dll version out of range." },
	{ 10093,	"Successful WSAStartup not yet performed." },
	{ 10101,	"Graceful shutdown in progress." },
	{ 10102,	"No more results." },
	{ 99999,	"Some another error occured..." }
};

WinsockInterface::WinsockInterface()
	: _wsaData(),
	  _sendingSocket(),
	  _receivingSocket(),
	  _sendingSocketAddress(),
	  _receivingSocketAddress(),
	  _addressInfo(nullptr),
	  _isRunning(false),
	  _wasInitialized(false),
	  _buffer(),
	  _message()
{
}

WinsockInterface::~WinsockInterface() noexcept
{
	if (_wasInitialized)
	{
		// The closing of the socket and Winsock.
		closesocket(_sendingSocket);
		closesocket(_receivingSocket);
		WSACleanup();
	}
}

void WinsockInterface::init()
{
	_wasInitialized = true;

	initWinsock(_wsaData);
	initSocket(_sendingSocket);
	initSocket(_receivingSocket);
}

void WinsockInterface::close()
{
	_wasInitialized = false;

	closesocket(_sendingSocket);
	closesocket(_receivingSocket);
	WSACleanup();

	_sendingSocket		= 0;
	_receivingSocket	= 0;
}

void WinsockInterface::initWinsock(WSADATA& wsaData) const
{
	// Initialization Winsock implementation.
	utils::print(std::cout, "Initializing Winsock...");

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		utils::println(std::cout, "\n\nFAILED. ERROR CODE:", WSAGetLastError());
		std::cin.get();
		assert(false);
		exit(static_cast<int>(ErrorType::FAILED_INITIALIZE_WSDATA));
	}

	utils::println(std::cout, " done.");
}

void WinsockInterface::initSocket(SOCKET& socketToInit, const int aiProtocol) const
{
	// Initialization socket.
	utils::println(std::cout, "Initialized socket.");

	socketToInit = socket(AF_INET, SOCK_STREAM, aiProtocol);

	if (socketToInit == INVALID_SOCKET)
	{
		utils::println(std::cout, "\nCOULD NOT CREATE SOCKET.");
		std::cin.get();
		assert(false);
		exit(static_cast<int>(ErrorType::FAILED_CREATE_SOCKET));
	}

	utils::println(std::cout, "Socket created.");

}

void WinsockInterface::bindSocket(const SOCKET& socketToBind, SOCKADDR_IN& socketAddress, 
								  const int port) const
{
	const u_short usPort = static_cast<u_short>(port);

	// Set socket settings.
	socketAddress.sin_family			= AF_INET;
	socketAddress.sin_addr.S_un.S_addr	= INADDR_ANY;
	socketAddress.sin_port				= htons(usPort);

	// Binding to a specific address and port.
	if (bind(socketToBind, reinterpret_cast<SOCKADDR*>(&socketAddress),
		sizeof socketAddress) == SOCKET_ERROR)
	{
		utils::println(std::cout, "\nBIND FAILED.");
		std::cin.get();
		assert(false);
		exit(static_cast<int>(ErrorType::FAILED_BIND));
	}

	utils::println(std::cout, "Bind done.");
}

void WinsockInterface::listenOn(const SOCKET& socketToList, const int backlog) const
{
	// Check if input data is correct (backlog should only positive value).
	assert(backlog > 0);

	// Include "listening" mode for receiving incoming connections.
	if (listen(socketToList, backlog) == SOCKET_ERROR)
	{
		utils::println(std::cout, "\nLISTEN FAILED.");
		std::cin.get();
		assert(false);
		exit(static_cast<int>(ErrorType::FAILED_LISTEN));
	}

	utils::println(std::cout, "Enabled listening.");

	// Cleaning addresses.
	freeaddrinfo(_addressInfo.get());
}

SOCKET WinsockInterface::acceptSocket(const SOCKET& listeningSocket)
{
	int addrLen = sizeof(SOCKADDR_IN);
	SOCKADDR_IN address;

	memset(_message, 0, _MAXRECV);

	const SOCKET socket = accept(listeningSocket, reinterpret_cast<SOCKADDR*>(&address),
								 static_cast<int*>(&addrLen));
	if (socket == SOCKET_ERROR)
	{
		perror("Accept failed.");
		std::cin.get();
		assert(false);
		exit(static_cast<int>(ErrorType::FAILED_ACCEPT_NEW_CLIENT));
	}

	// Get IP address back and print it.
	inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);

	// Inform user of socket number — used in send and receive commands.
	utils::println(std::cout, "New connection, socket FD is", socket, ", ip is:",
				   _message, ", PORT:", ntohs(address.sin_port));

	return socket;
}

bool WinsockInterface::tryConnect(const int port, const std::string& ip, 
								  const SOCKET& socketToConnect, SOCKADDR_IN& socketAddress) const
{
	const char* serverIP	= ip.c_str();
	const u_short usPort	= static_cast<u_short>(port);

	// Set socket settings.
	socketAddress.sin_family	= AF_INET;
	socketAddress.sin_port		= htons(usPort);
	inet_pton(AF_INET, serverIP, &socketAddress.sin_addr);

	// The connection to the server.
	if (connect(socketToConnect, reinterpret_cast<SOCKADDR*>(&socketAddress),
		sizeof socketAddress) == SOCKET_ERROR)
	{
		utils::println(std::cout, "\nCONNECTION TO SERVER WAS FAILED.");

		return false;
	}

	utils::println(std::cout, "Connected successfully.\n");
	
	return true;
}

bool WinsockInterface::isRun() const
{
	return _isRunning;
}

void WinsockInterface::sendData(const SOCKET& socketForSending, const std::string& data) const
{
	const char* dataChar = data.c_str();

	// Sending data on socket.
	if (send(socketForSending, dataChar, strlen(dataChar), 0) == SOCKET_ERROR)
	{
		utils::println(std::cout, "SEND FAILED.");
		return;
	}

	utils::println(std::cout, "Sent data:", data, "successfully.\n");
}

std::string WinsockInterface::receiveData(const SOCKET socketForReceiving)
{
	std::string result;

	int addrlen = sizeof(SOCKADDR_IN);

	SOCKADDR_IN address;

	memset(_message, 0, _MAXRECV);
	memset(_buffer, 0, _MAXRECV);

	// Get details of the client.
	getpeername(socketForReceiving, reinterpret_cast<SOCKADDR*>(&address),
				static_cast<int*>(&addrlen));

	const int valRead	= recv(socketForReceiving, _buffer, _MAXRECV, 0);
	const u_short port	= ntohs(address.sin_port);

	// Get IP address back and print it.
	inet_ntop(AF_INET, &address.sin_addr, _message, INET_ADDRSTRLEN);

	if (valRead == SOCKET_ERROR)
	{
		const int errorCode = WSAGetLastError();

		if (errorCode == WSAECONNRESET)
		{
			// Node disconnected, get his details and print.
			utils::println(std::cout, "Node disconnected unexpectedly, IP", _message, ", PORT", 
						   port);
		}
		else
		{
			utils::println(std::cout, "recv failed with error code:", errorCode);
		}

		_isRunning	= false;
		result		= "";
	}
	else if (valRead == 0)
	{
		// Node disconnected, get his details and print.
		utils::println(std::cout, "Node disconnected, IP", _message, ", PORT", port);

		_isRunning	= false;
		result		= "";
	}
	// Process message that came in.
	else if (valRead > 0 && valRead < _MAXRECV)
	{
		// Add null character, if you want to use with printf/puts or other string 
		// handling functions.
		_buffer[valRead] = '\0';

		utils::println(std::cout, _message, ':', port, '-', _buffer);

		result = _buffer;
	}

	return result;
}

void WinsockInterface::setTimeout(const SOCKET& socketToChange, const long seconds,
								  const long microseconds) const
{
	TIMEVAL timeout;
	timeout.tv_sec  = seconds;
	timeout.tv_usec = microseconds;
	setsockopt(socketToChange, SOL_SOCKET, SO_RCVTIMEO,
			   reinterpret_cast<char*>(&timeout), sizeof timeout);
}

}
