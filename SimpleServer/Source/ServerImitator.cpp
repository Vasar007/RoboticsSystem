#include <thread>

#include "ServerImitator.h"


namespace vasily
{

ServerImitator::ServerImitator(const int sendingPort, const int recivingPort, const int backlog)
	: WinsockInterface(),
	  _sendingPort(sendingPort),
	  _receivingPort(recivingPort),
	  _backlog(backlog),
	  _clientSendingSocket(0),
	  _clientReceivingSocket(0),
	  _hasGotCoordSystem(false),
	  _logger(_DEFAULT_IN_FILE_NAME, _DEFAULT_OUT_FILE_NAME),
	  _lastReceivedData({ RobotData::DEFAULT_CORDINATES }, { RobotData::DEFAULT_PARAMETERS })
{
}

ServerImitator::ServerImitator(ServerImitator&& other) noexcept
	: WinsockInterface(),
	  _sendingPort(other._sendingPort),
	  _receivingPort(other._receivingPort),
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
	utils::println(std::cout, "\n\n\nWaiting for connections...\n");
	_logger.write("\n\nServer waiting for connections at", utils::getCurrentSystemTime());

	while (!_isRunning)
	{
		_clientReceivingSocket  = acceptSocket(_receivingSocket);
		_clientSendingSocket    = acceptSocket(_sendingSocket);
		_hasGotCoordSystem		= false;

		if (!_isRunning)
		{
			_isRunning = true;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10LL));
	}
}

void ServerImitator::waitLoop()
{
	utils::println(std::cout, "\n\n\nWaiting for reply...\n");

	waitingForConnections();

	_logger.write("Server started to receive at", utils::getCurrentSystemTime());

	while (true)
	{
		std::string dataBuffer = receiveData(_clientReceivingSocket);

		if (!_isRunning)
		{
			waitingForConnections();
			continue;
		}

		if (!_hasGotCoordSystem && !dataBuffer.empty())
		{
			const std::string coordSystemStr = dataBuffer.substr(0u, 1u);
			utils::println(std::cout, coordSystemStr);
			_hasGotCoordSystem = true;
		}

		_logger.write(_message, '-', dataBuffer);
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
			dataBuffer.clear();
		}
	}
}

void ServerImitator::run()
{
	_isRunning = true;
	waitLoop();
}

void ServerImitator::launch()
{
	bindSocket(_sendingSocket, _sendingSocketAddress, _sendingPort);
	bindSocket(_receivingSocket, _receivingSocketAddress, _receivingPort);

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

	_isRunning = false;

	std::thread processThread(&ServerImitator::process, this);
	processThread.join();
}

std::chrono::milliseconds ServerImitator::calculateDuration(const RobotData& robotData)
{
	const double distance   = std::abs(_lastReceivedData.length() - robotData.length());
	_lastReceivedData       = robotData;

	constexpr long long multiplier  = 30LL;
	const long long result          = static_cast<long long>(distance * multiplier);

	return std::chrono::milliseconds(result);
}

}
