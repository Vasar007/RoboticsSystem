#include "TestServer.h"


namespace clientTests
{

TestServer::TestServer(const int sendingPort, const int recivingPort, const int backlog)
	: ServerImitator(sendingPort, recivingPort, backlog),
	  mHasConnected(false),
	  mHasFinished(false),
	  mStorage()
{
}

void TestServer::receiveDataNTimes(const int numberOfTimes)
{
	std::lock_guard<std::mutex> lockGuard{ mMutex };

	waitingForConnections();
	if (_clientReceivingSocket != 0 && _clientSendingSocket != 0)
	{
		mHasConnected = true;
	}

	for (int step = 0; step < numberOfTimes; ++step)
	{
		std::string dataBuffer = receiveData(_clientReceivingSocket);

		if (!_isRunning)
		{
			waitingForConnections();
		}

		if (!_hasGotCoordSystem && !dataBuffer.empty())
		{
			const std::string coordSystemStr = dataBuffer.substr(0u, 1u);
			_hasGotCoordSystem = true;
		}

		if (dataBuffer.size() == 1u)
		{
			continue;
		}

		if (std::count(dataBuffer.begin(), dataBuffer.end(), ' ') > 9)
		{
			mStorage = utils::fsplit<std::vector<std::string>>(dataBuffer, " 10 2 0 ");
		}
		else
		{
			mStorage.push_back(dataBuffer);
		}


		std::string toSending = utils::parseFullData(dataBuffer);
		if (!toSending.empty())
		{
			sendData(_clientSendingSocket, toSending);
		}

	}

	mHasFinished = true;
}

} // namespace clientTests
