#include <thread>

#include "ServerImitator.h"


namespace vasily
{

ServerImitator::ServerImitator(const int clientSendingPort, const int clientRecivingPort,
							   const int backlog)
	: WinsockInterface(),
	  _buffer(),
	  _messageWithIP(),
	  _clientSendingPort(clientSendingPort),
	  _clientReceivingPort(clientRecivingPort),
	  _backlog(backlog),
	  _clientSendingSocket(0),
	  _clientReceivingSocket(0),
	  _hasGotCoordSystem(false),
	  _logger(_DEFAULT_IN_FILE_NAME, _DEFAULT_OUT_FILE_NAME),
	  _lastReceivedData{ { RobotData::DEFAULT_CORDINATES }, { RobotData::DEFAULT_PARAMETERS } }
{
}

ServerImitator::ServerImitator(ServerImitator&& other) noexcept
	: WinsockInterface(),
	  _buffer(),
	  _messageWithIP(),
	  _clientSendingPort(other._clientSendingPort),
	  _clientReceivingPort(other._clientReceivingPort),
	  _backlog(other._backlog),
	  _clientSendingSocket(0),
	  _clientReceivingSocket(0),
	  _hasGotCoordSystem(other._hasGotCoordSystem ? true : false),
	  _logger(_DEFAULT_IN_FILE_NAME, _DEFAULT_OUT_FILE_NAME),
	  _lastReceivedData(other._lastReceivedData)
{
	utils::swap(*this, other);
}

ServerImitator& ServerImitator::operator=(ServerImitator&& other) noexcept
{
	if (this != &other)
	{
		utils::swap(*this, other);
	}
	return *this;
}

void ServerImitator::process()
{
	_printer.writeLine(std::cout, "\n\n\nWaiting for connections...\n");
	_logger.writeLine("\n\nServer waiting for connections at", utils::getCurrentSystemTime());

	while (!_isRunning.load())
	{
		_clientReceivingSocket  = acceptSocket(_receivingSocket, _messageWithIP);
		_clientSendingSocket    = acceptSocket(_sendingSocket, _messageWithIP);
		_hasGotCoordSystem.store(false);

		if (!_isRunning.load())
		{
			_isRunning.store(true);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10LL));
	}
}

void ServerImitator::waitLoop()
{
	_printer.writeLine(std::cout, "\n\n\nWaiting for reply...\n");

	waitingForConnections();

	_logger.writeLine("Server started to receive at", utils::getCurrentSystemTime());

	while (true)
	{
		std::string dataBuffer = receiveData(_clientReceivingSocket, _messageWithIP, _buffer);

		if (!_isRunning.load())
		{
			waitingForConnections();
			continue;
		}

		if (!_hasGotCoordSystem.load() && !dataBuffer.empty())
		{
			const std::string coordSystemStr = dataBuffer.substr(0u, 1u);
			_printer.writeLine(std::cout, coordSystemStr);
			_hasGotCoordSystem.store(true);
		}

		_logger.writeLine(_messageWithIP, '-', dataBuffer);
		std::string toSending = utils::parseFullData(dataBuffer);

		bool flag;
		const RobotData robotData = utils::fromString<RobotData>(dataBuffer, flag);
		if (flag)
		{
			std::this_thread::sleep_for(calculateDuration(robotData));
		}

		if (!toSending.empty())
		{
			sendData(_clientSendingSocket, toSending);
		}
	}
}

void ServerImitator::run()
{
	_isRunning.store(true);
	waitLoop();
}

void ServerImitator::launch()
{
	bindSocket(_sendingSocket, _sendingSocketAddress, _clientSendingPort);
	bindSocket(_receivingSocket, _receivingSocketAddress, _clientReceivingPort);

	listenOn(_sendingSocket, _backlog);
	listenOn(_receivingSocket, _backlog);
}

void ServerImitator::waitingForConnections()
{
	// Close the socket and mark as 0 for reuse.
	closesocket(_clientReceivingSocket);
	_clientReceivingSocket = 0;
	closesocket(_clientSendingSocket);
	_clientSendingSocket = 0;

	_isRunning.store(false);

	std::thread processThread(&ServerImitator::process, this);
	processThread.join();
}

std::chrono::milliseconds ServerImitator::calculateDuration(const RobotData& robotData)
{
	// Calculate distance between two points, which contains only first 3 coordinates.
	const double distance   = utils::distance(_lastReceivedData.coordinates.begin(),
											  _lastReceivedData.coordinates.begin() + 2,
											  robotData.coordinates.begin(), 0., 10'000.);
	_lastReceivedData       = robotData;

	constexpr long long multiplier  = 65LL;
	const long long result          = static_cast<long long>(distance * multiplier);

	return std::chrono::milliseconds(result);
}

} // namespace vasily
