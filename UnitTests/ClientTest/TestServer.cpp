#include "TestServer.h"


namespace clientTests
{

TestServer::TestServer(const int sendingPort, const int recivingPort, const int backlog)
	: ServerImitator(sendingPort, recivingPort, backlog)
	, mHasConnected(false)
	, mHasFinished(false)
	, mStorage()
{
}

void TestServer::receiveDataNTimes(const std::size_t numberOfTimes)
{
	std::lock_guard<std::mutex> lockGuard(mMutex);

	waitingForConnections();
	if (_clientSocketReceive != 0 && _clientSocketSend != 0)
	{
		mHasConnected = true;
	}

	std::string dataBuffer;
	std::string toSending;
	for (std::size_t step = 0u; step < numberOfTimes; ++step)
	{
		dataBuffer = receiveData(_clientSocketReceive);

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
			utils::fsplit(dataBuffer, mStorage, " 10 2 0 ");
		}
		else
		{
			mStorage.push_back(dataBuffer);
		}


		toSending = utils::parseData(dataBuffer);
		if (!toSending.empty())
		{
			sendData(_clientSocketSend, toSending);
		}

	}

	mHasFinished = true;
}

}
