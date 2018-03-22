#include "ServerLayer.h"


namespace vasily
{

inline const config::NamedConfig ServerLayer::_CONFIG
{
    { "DEFAULT_IN_FILE_NAME",               std::string{ "distance_to_time.txt" } },
    { "DEFAULT_OUT_FILE_NAME",              std::string{ "out.txt" } },
    { "DEFAULT_SERVER_IP",                  std::string{ "192.168.1.21" } },
    { "DEFAULT_SENDING_PORT_TO_SERVER",     int{ 59002 } },
    { "DEFAULT_RECEIVING_PORT_FROM_SERVER", int{ 59003 } },
    { "DEFAULT_CLIENT_PORT",                int{ 8888 } },
    { "NUMBER_OF_MAIN_COORDINATES",         std::size_t{ 3u } },
    { "RECONNECTION_DELAY",                 long long { 1000LL } },
    { "MIN_COORDINATES",                    std::array<int, 3u>{ 830'000, -400'000, 539'000 } },
    { "MAX_COORDINATES",                    std::array<int, 3u>{ 1'320'000, 400'000, 960'000 } }
};

ServerLayer::ServerLayer(const int serverSendingPort, const int serverRecivingPort,
                         const std::string_view serverIP, const int layerPort, const int backlog,
                         const WorkMode workMode)
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
      _workMode(workMode),
      _logger(_CONFIG.get<std::string>("DEFAULT_IN_FILE_NAME"),
              _CONFIG.get<std::string>("DEFAULT_OUT_FILE_NAME")),
      _delayManager(_printer, _logger)
{
}

ServerLayer::~ServerLayer()
{
    closeSocket(_layerSocket);
}

void ServerLayer::receiveFromServer()
{
    _printer.writeLine(std::cout, "\nReceiving thread for server started...\n");

    while (true)
    {
        const auto [dataBuffer, flag] = receiveData(_receivingSocket, _messageWithIP, _buffer);
        _isRunning.store(flag);

        if (!_isRunning.load())
        {
            tryReconnectToServer();
            continue;
        }

        if (!dataBuffer.empty())
        {
            sendData(_clientSocket, dataBuffer);
        }
        _logger.writeLine(_messageWithIP, '-', dataBuffer);
    }
}

void ServerLayer::checkConnectionToServer(const long long& time)
{
    while (true)
    {
        _logger.writeLine(_lastReceivedPoint);
        sendData(_sendingSocket, _lastReceivedPoint.toString());

        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }
}

void ServerLayer::receiveFromClients()
{
    _printer.writeLine(std::cout, "\nReceive thread for clients started...\n");

    while (true)
    {
        const auto [dataBuffer, status] = receiveData(_clientSocket, _messageWithIPForClient,
                                                      _bufferForClient);
        _logger.writeLine(_messageWithIPForClient, '-', dataBuffer);
        _isRunningForClient.store(status);

        if (!_isRunningForClient.load())
        {
            waitingForConnections();
            continue;
        }
        if (dataBuffer.empty())
        {
            continue;
        }

        switch (_workMode)
        {
            case WorkMode::SAFE:
            {
                if (!_coorninateSystem.has_value())
                {
                    break;
                }

                bool flag;
                const auto robotData = utils::fromString<RobotData>(dataBuffer, flag);
                if (!flag || !checkCoordinates(robotData))
                {
                    sendData(_clientSocket, "INCORRECT COORDINATES: " + dataBuffer);
                    continue;
                }
                break;
            }
            
            case WorkMode::UNSAFE:
                _printer.writeLine(std::cout, "Warning: working in unsafe mode!");
                break;

            default:
                assert(false);
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

bool ServerLayer::checkCoordinates(const RobotData& robotData) const
{
    // Get default parameters for checking.
    static const auto kMainCoordinates = _CONFIG.get<std::size_t>("NUMBER_OF_MAIN_COORDINATES");
    static const auto kMinCoords = _CONFIG.get< std::array<int, 3u> >("MIN_COORDINATES");
    static const auto kMaxCoords = _CONFIG.get< std::array<int, 3u> >("MAX_COORDINATES");

    for (std::size_t i = 0u; i < kMainCoordinates; ++i)
    {
        if (robotData.coordinates.at(i) < kMinCoords.at(i)
            || robotData.coordinates.at(i) > kMaxCoords.at(i))
        {
            _printer.writeLine(std::cout, "ERROR 03: Incorrect coordinates to send!", robotData);
            return false;
        }
    }

    return true;
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
            // Not move because RobotData is LiteralType.
            const RobotData robotData = _messagesStorage.front();
            sendData(_sendingSocket, robotData.toString());

            std::this_thread::sleep_for(_delayManager.calculateDuration(_lastReceivedPoint,
                                                                        robotData));
            _lastReceivedPoint = robotData;
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

        std::this_thread::sleep_for(
            std::chrono::milliseconds(_CONFIG.get<long long>("RECONNECTION_DELAY")));
    }
}

} // namespace vasily
