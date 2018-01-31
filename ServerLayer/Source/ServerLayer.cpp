#include "ServerLayer.h"


namespace vasily
{

ServerLayer::ServerLayer(const int serverSendingPortR, const int serverRecivingPort,
						 const std::string_view serverIP, const int layerPort, const int backlog)
	: WinsockInterface(),
	  _bufferForServer(),
	  _messageWithIPForServer(),
	  _bufferForClient(),
	  _messageWithIPForClient(),
	  _serverIP(serverIP),
	  _layerPort(layerPort),
	  _backlog(backlog),
	  _clientSocket(0),
	  _layerSocket(),
	  _layerSocketAddress(),
	  _serverSendingPort(serverSendingPortR),
	  _serverReceivingPort(serverRecivingPort),
	  _robotData(), 
	  _logger(_DEFAULT_IN_FILE_NAME, _DEFAULT_OUT_FILE_NAME)
{
}

ServerLayer::~ServerLayer() noexcept
{
	closesocket(_layerSocket);
	_layerSocket = 0;
}


void ServerLayer::receiveFromServer()
{
	_printer.writeLine(std::cout, "\nReceiving thread started...\n");

	int count = 0;
	while (true)
	{
		const std::string dataBuffer = receiveData(_receivingSocket, _messageWithIPForServer,
												   _bufferForServer);

		if (!_isRunning.load())
		{
			tryReconnect();
			continue;
		}

		++count;
		_logger.writeLine(_messageWithIPForServer, '-', dataBuffer);

		if (!dataBuffer.empty())
		{
			sendData(_clientSocket, dataBuffer);
		}
	}
}

void ServerLayer::checkConnection(const std::atomic_int64_t& time)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1000LL));

	while (true)
	{
		_logger.writeLine(_robotData);
		sendData(_sendingSocket, _robotData.toString());

		std::this_thread::sleep_for(std::chrono::milliseconds(time.load()));
	}
}

void ServerLayer::process()
{
	_printer.writeLine(std::cout, "\nWaiting for connections...\n");
	_logger.writeLine("\nServer layer waiting for connections at", utils::getCurrentSystemTime());

	while (!_isRunning.load())
	{
		_clientSocket = acceptSocket(_layerSocket, _messageWithIPForClient);

		if (!_isRunning.load())
		{
			_isRunning.store(true);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10LL));
	}
}

void ServerLayer::waitLoop()
{
	_printer.writeLine(std::cout, "\n\nLaunched layer...\n");

	std::thread reciveThread(&ServerLayer::receiveFromServer, this);
	reciveThread.detach();

	waitingForConnections();

	_logger.writeLine("Server layer started to receive at", utils::getCurrentSystemTime());

	while (true)
	{
		std::string dataBuffer = receiveData(_clientSocket, _messageWithIPForClient,
											 _bufferForClient);

		if (!_isRunning.load())
		{
			waitingForConnections();
			continue;
		}

		_logger.writeLine(_messageWithIPForClient, '-', dataBuffer);

		if (!dataBuffer.empty())
		{
			sendData(_sendingSocket, dataBuffer);
		}
	}
}

void ServerLayer::run()
{
	_isRunning.store(true);
	waitLoop();
}

void ServerLayer::launch()
{
	initSocket(_layerSocket);

	bindSocket(_layerSocket, _layerSocketAddress, _layerPort);

	listenOn(_layerSocket, _backlog);
	
	tryConnect(_serverSendingPort, _serverIP, _sendingSocket, _sendingSocketAddress);
	tryConnect(_serverReceivingPort, _serverIP, _receivingSocket, _receivingSocketAddress);
}

void ServerLayer::waitingForConnections()
{
	// Close the socket and mark as 0 for reuse.
	closesocket(_clientSocket);
	_clientSocket = 0;

	_isRunning.store(false);

	std::thread processThread(&ServerLayer::process, this);
	processThread.join();
}

std::string ServerLayer::getServerIP() const
{
	return _serverIP;
}

void ServerLayer::setServerIP(const std::string_view newServerIP)
{
	_serverIP = newServerIP;
}

void ServerLayer::tryReconnect()
{
	while (!_isRunning.load())
	{
		closesocket(_sendingSocket);
		closesocket(_receivingSocket);

		initSocket(_sendingSocket);
		initSocket(_receivingSocket);

		const bool reconnect = tryConnect(_serverSendingPort, _serverIP, _sendingSocket, 
										  _sendingSocketAddress)
								&& tryConnect(_serverReceivingPort, _serverIP, _receivingSocket,
											  _receivingSocketAddress);
		_isRunning.store(reconnect);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000LL));
	}
}

} // namespace vasily
