#include "TestServer.h"


namespace clientTests
{

inline const config::NamedConfig TestServer::CONFIG
{
    { "PORT_SENDING",   int{ 9998 } },
    { "PORT_RECEIVING", int{ 9999 } },
};

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
        const auto [dataBuffer, flag] = receiveData(_clientReceivingSocket, _messageWithIP,
                                                    _buffer);
        _isRunning.store(flag);

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
