#ifndef WINSOCK_INTERFACE_H
#define WINSOCK_INTERFACE_H
#pragma once

#include <map>
#include <string>
#include <vector>
#include <winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


class WinsockInterface
{
	public:
		enum ErrorType
		{
			failedInitializeWsdata = -1,
			failedCreateSocket = -2,
			failedBind = -3,
			failedAccept = -4,
			failedListen = -5,
			failedSelectCall = -6,
			failedAcceptNewClient = -7
		};
	
	
	public:
		SOCKET			 _masterSocket;
	
		WSADATA			 _wsaData;
	
		sockaddr_in		 _serverAddress;
	
		bool			_isRunning;
	
		// Size of our receive buffer, this is string length.
		const int		 _MAXRECV = 1024;
	
		char*			 _buffer;
	
		char*			 _message;
	
		const std::map<int, std::string> _tableOfErrors =
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
			{ -1,	 "Some another error occured..." }
		};

	
	protected:
		void initWinsock();
		void initSocket(int aiProtocol = 6);
	
		void bindSocket(int port);
		void listenOn(int backlog = 10) const;
		bool tryConnect(int port, std::string ip);

		void sendData(std::string data) const;

		virtual void waitLoop() = 0;

		bool isRun() const;
	
	
	public:
		WinsockInterface();
		virtual ~WinsockInterface();
	
		virtual void run() = 0;
};

#endif // WINSOCK_INTERFACE_H
