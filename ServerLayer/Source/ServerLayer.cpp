#include "ServerLayer.h"


namespace vasily
{

ServerLayer::ServerLayer(const int serverSendingPort, const int serverRecivingPort,
						 const std::string_view serverIP, const int layerPort, const int backlog)
	: _bufferForClient(),
	  _messageWithIPForClient(),
	  _isRunningForClient(false),
	  _serverIP(serverIP),
	  _layerPort(layerPort),
	  _backlog(backlog),
	  _clientSocket(0),
	  _layerSocket(),
	  _layerSocketAddress(),
	  _serverSendingPort(serverSendingPort),
	  _serverReceivingPort(serverRecivingPort),
	  _logger(_DEFAULT_IN_FILE_NAME, _DEFAULT_OUT_FILE_NAME)
{
	_hasReadTable = readDataTable();
}

ServerLayer::~ServerLayer() noexcept
{
	closeSocket(_layerSocket);
}

bool ServerLayer::readDataTable()
{
	while (!_logger.inFile.eof())
	{
		if (_logger.hasAnyInputErrors())
		{
			_printer.writeLine(std::cout, "Failed to read data from table!");
			return false;
		}

		const auto distance = _logger.read<long long>();
		const auto time     = _logger.read<long long>();
		_distanceToTimeTable.emplace_hint(_distanceToTimeTable.end(), distance, time);
	}

	return !_distanceToTimeTable.empty();
}

void ServerLayer::receiveFromServer()
{
	_printer.writeLine(std::cout, "\nReceiving thread for server started...\n");

	while (true)
	{
		bool flag = _isRunning.load();
		const std::string dataBuffer = receiveData(_receivingSocket, _messageWithIP, _buffer, flag);
		_isRunning.store(flag);

		if (!_isRunning.load())
		{
			tryReconnectToServer();
			continue;
		}

		_logger.writeLine(_messageWithIP, '-', dataBuffer);

		if (!dataBuffer.empty())
		{
			sendData(_clientSocket, dataBuffer);
		}
	}
}

void ServerLayer::checkConnectionToServer(const long long& time)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1000LL));

	while (true)
	{
		_logger.writeLine(_lastReceivedPoint);
		sendData(_sendingSocket, _lastReceivedPoint.toString());

		std::this_thread::sleep_for(std::chrono::milliseconds(time));
	}
}

std::chrono::milliseconds ServerLayer::calculateDuration(const RobotData& robotData)
{	
	if (!_hasReadTable)
	{
		_printer.writeLine(std::cout, "No duration!");
		return std::chrono::milliseconds(0LL);
	}

	// Calculate distance between two points, which contains only first 3 coordinates.
	const long long distance    = utils::distance(_lastReceivedPoint.coordinates.begin(),
												  _lastReceivedPoint.coordinates.begin() + 2,
												  robotData.coordinates.begin(), 0LL, 1LL);
	_lastReceivedPoint          = robotData;

	const auto low = _distanceToTimeTable.lower_bound(distance);
	
	if (low == _distanceToTimeTable.end())
	{
		const auto it = --_distanceToTimeTable.end();
		_printer.writeLine(std::cout, "Distance:", it->first, ", Duration:", it->second.count());
		return it->second;
	}
	if (low == _distanceToTimeTable.begin())
	{
		_printer.writeLine(std::cout, "Distance:", low->first, ", Duration:", low->second.count());
		return low->second;
	}
	
	auto prev = low;
	--prev;
	if (distance - prev->first < low->first - distance)
	{
		_printer.writeLine(std::cout, "Distance:", prev->first,
						   ", Duration:", prev->second.count());
		return prev->second;
	}

	_printer.writeLine(std::cout, "Distance:", low->first, ", Duration:", low->second.count());
	return low->second;
}

void ServerLayer::receiveFromClients()
{
	_printer.writeLine(std::cout, "\nReceive thread for clients started...\n");

	while (true)
	{
		bool flag = _isRunningForClient.load();
		const std::string dataBuffer = receiveData(_clientSocket, _messageWithIPForClient,
												   _bufferForClient, flag);
		_logger.writeLine(_messageWithIPForClient, '-', dataBuffer);
		_isRunningForClient.store(flag);

		if (!_isRunningForClient.load())
		{
			waitingForConnections();
			continue;
		}

		if (dataBuffer.empty())
		{
			continue;
		}

		std::lock_guard<std::mutex> lockGuard{ _mutex };
		if (const auto [value, check] = utils::parseCoordinateSystem(dataBuffer); check)
		{
			sendData(_sendingSocket, dataBuffer);
			_coorninateSystem.emplace(value);
		}
		else if (_messagesStorage.empty())
		{
			_messagesStorage = utils::parseData(dataBuffer);
		}
		else
		{
			for (auto&& datum : utils::parseData(dataBuffer))
			{
				_messagesStorage.emplace_back(datum);
			}
		}
	}
}

void ServerLayer::process()
{
	_printer.writeLine(std::cout, "\nWaiting for connections...\n");
	_logger.writeLine("\nServer layer waiting for connections at", utils::getCurrentSystemTime());

	while (!_isRunningForClient.load())
	{
		_clientSocket = acceptSocket(_layerSocket, _messageWithIPForClient);
		_isRunningForClient.store(true);

		std::this_thread::sleep_for(std::chrono::milliseconds(10LL));
	}
}

void ServerLayer::waitLoop()
{
	_printer.writeLine(std::cout, "\n\nLaunched layer...\n");

	std::thread reciveThreadFromServer(&ServerLayer::receiveFromServer, this);
	reciveThreadFromServer.detach();

	waitingForConnections();

	std::thread reciveThreadFromClients(&ServerLayer::receiveFromClients, this);
	reciveThreadFromClients.detach();

	_logger.writeLine("Server layer started to receive at", utils::getCurrentSystemTime());

	while (true)
	{
		std::lock_guard<std::mutex> lockGuard{ _mutex };
		while (!_messagesStorage.empty())
		{
			const RobotData robotData = _messagesStorage.front();
			sendData(_sendingSocket, robotData.toString());

			std::this_thread::sleep_for(calculateDuration(robotData));
			_messagesStorage.pop_front();
		}
	}
}

void ServerLayer::doConnection()
{
	const bool connectStatus = tryConnect(_serverSendingPort, _serverIP, _sendingSocket,
										  _sendingSocketAddress)
								&& tryConnect(_serverReceivingPort, _serverIP, _receivingSocket,
											  _receivingSocketAddress);
	_isRunning.store(connectStatus);
}

void ServerLayer::run()
{
	_isRunning.store(true);
	_isRunningForClient.store(true);
	waitLoop();
}

void ServerLayer::launch()
{
	initSocket(_layerSocket);
	bindSocket(_layerSocket, _layerSocketAddress, _layerPort);

	listenOn(_layerSocket, _backlog);
	doConnection();
}

void ServerLayer::waitingForConnections()
{
	closeSocket(_clientSocket);
	_isRunningForClient.store(false);
	process();
}

std::string ServerLayer::getServerIP() const
{
	return _serverIP;
}

void ServerLayer::setServerIP(const std::string_view newServerIP)
{
	_serverIP = newServerIP;
}

void ServerLayer::tryReconnectToServer()
{
	while (!_isRunning.load())
	{
		closeSocket(_sendingSocket);
		closeSocket(_receivingSocket);

		initSocket(_sendingSocket);
		initSocket(_receivingSocket);

		doConnection();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000LL));
	}
}

} // namespace vasily
