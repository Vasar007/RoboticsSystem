#include "TestServer.h"


namespace clientTests
{

TestServer::TestServer(const int sendingPort, const int recivingPort, const int backlog)
	: ServerImitator(sendingPort, recivingPort, backlog),
	  hasConnected(false),
	  hasFinished(false),
	  storage()
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
		std::string dataBuffer = receiveData(_clientReceivingSocket);

		if (!_isRunning)
		{
			waitingForConnections();
		}

		if (!_hasGotCoordSystem.load() && !dataBuffer.empty())
		{
			const std::string coordSystemStr = dataBuffer.substr(0u, 1u);
			_hasGotCoordSystem.store(true);
		}

		if (dataBuffer.size() == 1u)
		{
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


		std::string toSending = utils::parseFullData(dataBuffer);
		if (!toSending.empty())
		{
			sendData(_clientSendingSocket, toSending);
		}

	}

	hasFinished.store(true);
}

} // namespace clientTests
