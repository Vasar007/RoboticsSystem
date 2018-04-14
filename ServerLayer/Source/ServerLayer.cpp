#include "ServerLayer.h"


namespace vasily
{

inline const config::Config<std::string, std::string, std::string_view, int, int, int,
                            std::size_t, std::array<int, 3>, std::array<int, 3>, long long>
    ServerLayer::CONFIG
{
    { "distance_to_time.txt" },
    { "out.txt" },
    { "172.27.221.60", 14 },
    59003,
    59002,
    8888,
    3,
    { 830'000,  -400'000, 539'000 },
    { 1'320'000, 400'000, 960'000 },
    1000
};

ServerLayer::ServerLayer(const int serverReceivingPort,  const int serverSendingPort,
                         const std::string_view serverIP, const int layerPort,
                         const WorkMode workMode, QObject* parent)
    : QObject(parent),
      _layerPort(layerPort),
      _layerSocket(new QTcpServer(this)),
      _clientSocket(nullptr),
      _serverReceivingPort(serverReceivingPort),
      _receivingSocket(new QTcpSocket(this)),
      _serverSendingPort(serverSendingPort),
      _sendingSocket(new QTcpSocket(this)),
      _serverIP(serverIP),
      _workMode(workMode),
      _logger(CONFIG.get<CAST(Param::DEFAULT_IN_FILE_NAME)>(),
              CONFIG.get<CAST(Param::DEFAULT_OUT_FILE_NAME)>()),
      _delayManager(_printer, _logger)
{
    _printer.writeLine(std::cout, "Server Receiving Port:", serverReceivingPort,
                       "Server Sending Port:", serverSendingPort, "Server IP:", serverIP,
                       "Layer Port:", layerPort);

    connect(_layerSocket.get(), &QTcpServer::newConnection, this,
            &ServerLayer::slotNewClientConnection);
    
    connect(this, &ServerLayer::signalToSendToClient, this, &ServerLayer::slotSendDataToClient);

    connect(_receivingSocket.get(), &QTcpSocket::readyRead, this, &ServerLayer::slotReadFromServer);
    connect(_receivingSocket.get(), &QTcpSocket::disconnected, this,
            &ServerLayer::slotServerDisconnected, Qt::QueuedConnection);

    connect(_sendingSocket.get(), &QTcpSocket::disconnected, this,
            &ServerLayer::slotServerDisconnected, Qt::QueuedConnection);

    connect(this, &ServerLayer::signalToSendToServer, this, &ServerLayer::slotSendDataToServer);

    connect(this, &ServerLayer::signalProcessMessagesStorage, this,
            &ServerLayer::slotProcessMessagesStorage);

    connect(this, &ServerLayer::signalProcessAnswersStorage, this,
            &ServerLayer::slotProcessAnswersStorage);

    _printer.writeLine(std::cout, "\nServerLayer launched...\n");
    _logger.writeLine("\nServerLayer launched at", utils::getCurrentSystemTime());
}

void ServerLayer::slotNewClientConnection()
{
    _printer.writeLine(std::cout, "\nWaiting for connections from Clients...\n");
    _logger.writeLine("\nServerLayer waiting for connections from Clients at",
                      utils::getCurrentSystemTime());

    _clientSocket = _layerSocket->nextPendingConnection();

    _clientSocket->write("Test message from ServerLayer::LayerSocket.");

    connect(_clientSocket, &QTcpSocket::readyRead, this, &ServerLayer::slotReadFromClient);
    connect(_clientSocket, &QTcpSocket::disconnected, this, &ServerLayer::slotClientDisconnected);

    _printer.writeLine(std::cout, "Client connected to layer port\n");
    _logger.writeLine("ServerLayer started to receive at", utils::getCurrentSystemTime());
}

void ServerLayer::slotClientDisconnected()
{
    _printer.writeLine(std::cout, "Client disconnected!");
    _clientSocket->close();
    _coorninateSystem.reset();
}

void ServerLayer::slotServerDisconnected()
{
    _printer.writeLine(std::cout, "\nServer disconnected!");
    tryReconnectToServer();
}

void ServerLayer::slotReadFromClient()
{
    if (_clientSocket->bytesAvailable() > 0)
    {
        QByteArray array = _clientSocket->readAll();
        ///qDebug() << array << '\n';

        const std::string receivedData = array.toStdString();
        _logger.writeLine(_clientSocket->localPort(), '-', receivedData); // messageWithIP

        switch (_workMode)
        {
            case WorkMode::SAFE:
            {
                if (!_coorninateSystem.has_value())
                {
                    break;
                }

                bool flag;
                const auto robotData = utils::fromString<RobotData>(receivedData, flag);
                if (!flag || !checkCoordinates(robotData))
                {
                    sendData("INCORRECT COORDINATES: " + receivedData, Whereto::CLIENT);
                }
                break;
            }
            
            case WorkMode::UNSAFE:
                _printer.writeLine(std::cout, "Warning: working in unsafe mode!");
                break;

            default:
                assert(false);
                break;
        }

        if (const auto [value, check] = utils::parseCoordinateSystem(receivedData); check)
        {
            sendData(receivedData, Whereto::SERVER);
            _coorninateSystem.emplace(value);
        }
        else if (_messagesStorage.empty())
        {
            _messagesStorage = utils::parseData(receivedData);
            emit signalProcessMessagesStorage();
        }
        else
        {
            for (auto&& datum : utils::parseData(receivedData))
            {
                _messagesStorage.emplace_back(datum);
            }
            emit signalProcessMessagesStorage();
        }
    }
}

void ServerLayer::slotReadFromServer()
{
    if (_receivingSocket->bytesAvailable() > 0)
    {
        const QByteArray array = _receivingSocket->readAll();
        const std::string data = array.toStdString();

        _answersStorage.push_back(data);
        ///sendData(data, Whereto::CLIENT);
        emit signalProcessAnswersStorage();

        _logger.writeLine(_receivingSocket->localPort(), '-', data);
    }
}

void ServerLayer::slotSendDataToClient(const QByteArray& data) const
{
    _clientSocket->write(data);
    _printer.writeLine(std::cout, "Sent data to client:", data.toStdString(), "successfully.\n");
}

void ServerLayer::slotSendDataToServer(const QByteArray& data) const
{
    _sendingSocket->write(data);
    _printer.writeLine(std::cout, "Sent data to server:", data.toStdString(), "successfully.\n");
}

void ServerLayer::slotProcessMessagesStorage()
{
    while (!_messagesStorage.empty())
    {
        // Not move because RobotData is LiteralType (in such case move == copy).
        const RobotData robotData = _messagesStorage.front();
        sendData(robotData.toString(), Whereto::SERVER);

        std::this_thread::sleep_for(_delayManager.calculateDuration(_lastReceivedPoint,
                                                                    robotData));
        _lastReceivedPoint = robotData;
        _messagesStorage.pop_front();
    }
}

void ServerLayer::slotProcessAnswersStorage()
{
    while (!_answersStorage.empty())
    {
        if (_clientSocket == nullptr)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        const std::string data = std::move(_answersStorage.front());
        sendData(data, Whereto::CLIENT);

        _answersStorage.pop_front();
    }
}

void ServerLayer::checkConnectionToServer(const long long time)
{
    while (true)
    {
        sendData(_lastReceivedPoint.toString(), Whereto::SERVER);
        _logger.writeLine(_lastReceivedPoint);

        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }
}

bool ServerLayer::checkCoordinates(const RobotData& robotData) const
{
    // Get default parameters for checking.
    static const std::size_t kMainCoordinates  = CONFIG.get<CAST(Param::NUMBER_OF_MAIN_COORDINATES)>();
    static const std::array<int, 3> kMinCoords = CONFIG.get<CAST(Param::MIN_COORDINATES)>();
    static const std::array<int, 3> kMaxCoords = CONFIG.get<CAST(Param::MAX_COORDINATES)>();

    for (std::size_t i = 0; i < kMainCoordinates; ++i)
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

void ServerLayer::sendData(const std::string& data, const Whereto recipient) const
{
    switch (recipient)
    {
        case Whereto::SERVER:
            emit signalToSendToServer(data.c_str());
            break;
        
        case Whereto::CLIENT:
            emit signalToSendToClient(data.c_str());
            break;
        
        default:
            assert(false);
            break;
    }
}

void ServerLayer::launch()
{
    if (_layerSocket->listen(QHostAddress::Any, _layerPort))
    {
        _printer.writeLine(std::cout, "Layer server is started!");
    }
    else
    {
        _printer.writeLine(std::cout, "Layer server is not started!");
    }
    doConnection();
}

bool ServerLayer::tryConnect(const int port, const std::string& ip, QTcpSocket* socketToConnect,
                             const int msecs) const
{
    if (socketToConnect->isOpen())
    {
        socketToConnect->close();
    }

    socketToConnect->connectToHost(ip.c_str(), port);

    if (socketToConnect->waitForConnected(msecs))
    {
        _printer.writeLine(std::cout, "Connected to Server!");
        return true;
    }

    _printer.writeLine(std::cout, "Not connected to Server!");
    return false;
}

void ServerLayer::doConnection()
{
    // Remember that sending socket should be connect to server receiving port
    // and receiving socket should be connect to server sending port!
    bool isConnected = tryConnect(_serverReceivingPort, _serverIP, _sendingSocket.get())
                    && tryConnect(_serverSendingPort, _serverIP, _receivingSocket.get());
    while (!isConnected)
    {
        isConnected = tryConnect(_serverReceivingPort, _serverIP, _sendingSocket.get())
                   && tryConnect(_serverSendingPort, _serverIP, _receivingSocket.get());

        std::this_thread::sleep_for(
            std::chrono::milliseconds(CONFIG.get<CAST(Param::RECONNECTION_DELAY)>()));
    }

    _printer.writeLine(std::cout, "\nServerLayer launched...\n");
    _logger.writeLine("\nServerLayer launched at", utils::getCurrentSystemTime());
}

void ServerLayer::tryReconnectToServer()
{
    doConnection();
}

} // namespace vasily
