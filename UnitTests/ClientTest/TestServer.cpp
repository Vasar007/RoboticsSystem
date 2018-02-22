#include "TestServer.h"


namespace clientTests
{

TestServer::TestServer(const int sendingPort, const int recivingPort, const int backlog)
	: ServerImitator(sendingPort, recivingPort, backlog),
	  hasConnected(false),
	  hasFinished(false)
{
}

void TestServer::receiveDataNTimes(const int numberOfTimes)
{
	std::lock_guard<std::mutex> lockGuard{ mutex };

	waitingForConnections();
	if (_clientReceivingSocket != 0 && _clientSendingSocket != 0)
	{
		hasConnected.store(true);
	}

	for (int step = 0; step < numberOfTimes; ++step)
	{
		const std::string dataBuffer = receiveData(_clientReceivingSocket, _messageWithIP, _buffer);

		if (!_isRunning)
		{
			waitingForConnections();
		}

		if (const auto [value, check] = utils::parseCoordinateSystem(dataBuffer); check)
		{

			_coorninateSystem.emplace(value);
			continue;
		}

		if (std::count(dataBuffer.begin(), dataBuffer.end(), ' ') > 9)
		{
			storage = utils::fsplit<std::vector<std::string>>(dataBuffer, " 10 2 0 ");
		}
		else
		{
			storage.push_back(dataBuffer);
		}

		if (const std::string toSending = utils::parseFullData(dataBuffer); !toSending.empty())
		{
			sendData(_clientSendingSocket, toSending);
		}

	}

	hasFinished.store(true);
}

} // namespace clientTests
