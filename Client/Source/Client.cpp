#include <cassert>
#include <thread>

#include "Client.h"


namespace vasily
{

inline const config::Config<std::string, std::string, std::string_view, int, int, long long>
    Client::CONFIG
{
    { "in.txt" },
    { "out.txt" },
    { "172.27.221.60", 14 },
    59002,
    59003,
    1000
};
 
Client::Client(const int layerPort, const std::string_view serverIP, const WorkMode workMode,
               QObject* parent)
    : QObject(parent),
      _layerPort(layerPort),
      _socketForLayer(std::make_unique<QTcpSocket>(this)),
      _serverReceivingPort(0),
      _receivingSocket(nullptr),
      _serverSendingPort(0),
      _sendingSocket(nullptr),
      _serverIP(serverIP),
      _start(std::chrono::steady_clock::now()),
      _workMode(workMode),
      _logger(CONFIG.get<Param::DEFAULT_IN_FILE_NAME>(),
              CONFIG.get<Param::DEFAULT_OUT_FILE_NAME>())
{
    _printer.writeLine(std::cout, "Layer Port:", layerPort, "Layer IP:", serverIP);

    connect(_socketForLayer.get(), &QTcpSocket::readyRead, this, &Client::slotReadFromLayer);

    connect(_socketForLayer.get(), &QTcpSocket::disconnected, this, &Client::slotServerDisconnected,
            Qt::QueuedConnection);

    connect(this, &Client::signalToSend, this, &Client::slotSendDataToLayer);
}

Client::Client(const int serverReceivingPort, const int serverSendingPort,
               const std::string_view serverIP, const WorkMode workMode, QObject* parent)
    : QObject(parent),
      _layerPort(0),
      _socketForLayer(nullptr),
      _serverReceivingPort(serverReceivingPort),
      _receivingSocket(std::make_unique<QTcpSocket>(this)),
      _serverSendingPort(serverSendingPort),
      _sendingSocket(std::make_unique<QTcpSocket>(this)),
      _serverIP(serverIP),
      _start(std::chrono::steady_clock::now()),
      _workMode(workMode),
      _logger(CONFIG.get<Param::DEFAULT_IN_FILE_NAME>(),
              CONFIG.get<Param::DEFAULT_OUT_FILE_NAME>())
{
    _printer.writeLine(std::cout, "Server Receiving Port:", serverReceivingPort,
                       "Server Sending Port:", serverSendingPort, "Server IP:", serverIP);

    connect(_receivingSocket.get(), &QTcpSocket::readyRead, this, &Client::slotReadFromServer);
    connect(_receivingSocket.get(), &QTcpSocket::disconnected, this,
            &Client::slotServerDisconnected, Qt::QueuedConnection);

    ///connect(_sendingSocket.get(), &QTcpSocket::readyRead, this, &Client::slotClientRead);
    connect(_sendingSocket.get(), &QTcpSocket::disconnected, this, &Client::slotServerDisconnected,
            Qt::QueuedConnection);

    connect(this, &Client::signalToSend, this, &Client::slotSendDataToServer);
}

void Client::slotReadFromLayer()
{
    if (_socketForLayer->bytesAvailable() > 0)
    {
        const QByteArray array = _socketForLayer->readAll();
        const std::string receivedData = array.toStdString();

        _duration = std::chrono::steady_clock::now() - _start;
        ///_start = std::chrono::steady_clock::now();

        _printer.writeLine(std::cout, _socketForLayer->localPort(), '-', receivedData);
        _logger.writeLine(_socketForLayer->localPort(), '-', receivedData);

        _printer.writeLine(std::cout, "Duration:", _duration.count(), "seconds");
        _logger.writeLine("Duration:", _duration.count(), "seconds");
    }
}

void Client::slotReadFromServer()
{
    if (_receivingSocket->bytesAvailable() > 0)
    {
        const QByteArray array = _receivingSocket->readAll();
        const std::string receivedData = array.toStdString();

        _duration = std::chrono::steady_clock::now() - _start;
        _isReceive.store(true);

        _printer.writeLine(std::cout, _receivingSocket->localPort(), '-', receivedData);
        _logger.writeLine(_receivingSocket->localPort(), '-', receivedData);

        _printer.writeLine(std::cout, "Duration:", _duration.count(), "seconds");
        _logger.writeLine("Duration:", _duration.count(), "seconds");
        updateVertices(_duration.count(), _robotData);
    }
}

void Client::slotServerDisconnected()
{
    _printer.writeLine(std::cout, "\nServer disconnected!");
    tryReconnect();
}

void Client::slotSendDataToLayer(const QByteArray& data) const
{
    _socketForLayer->write(data);
    _printer.writeLine(std::cout, "Sent data:", data.toStdString(), "successfully.\n");
}

void Client::slotSendDataToServer(const QByteArray& data) const
{
    _sendingSocket->write(data);
    _printer.writeLine(std::cout, "Sent data:", data.toStdString(), "successfully.\n");
}

void Client::checkConnection(const long long time)
{
    while (true)
    {
        sendCoordinates(_robotData);
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }
}

void Client::updateVertices(const double time, const vasily::RobotData& robotData)
{
  
    const double distance = utils::distance(_lastReachedPoint.coordinates.begin(),
                                            _lastReachedPoint.coordinates.begin() + 2,
                                            robotData.coordinates.begin(), 0.0, 10'000.0);
    _distance.emplace_back(distance);
    _velocity.emplace_back(_distance.back() / time);
    _time.emplace_back(time);
    _lastReachedPoint = robotData;
}

void Client::run()
{
    std::thread workThread(&Client::waitLoop, this);
    workThread.detach();
}

void Client::waitLoop()
{
    _printer.writeLine(std::cout, "\nWaiting for reply...\n");

    while (true)
    {
        std::string input;
        // TODO(Vasily, Danila): Improved this block.
        switch (_handler.getCurrentMode())
        {
            case Handler::Mode::READING:
                _printer.write(std::cout, "Enter coordinates or to change mode enter '=':");
                std::getline(std::cin, input);

                _handler.parseRawData(input, _robotData);

                if (_handler.getCurrentState() == Handler::State::COORDINATE_TYPE)
                {
                    sendCoordinateSystem(_handler.getCoordinateSystem());
                }
                else if (_handler.getCurrentState() == Handler::State::FULL_CONTROL)
                {
                    sendCoordinates(_robotData);
                }
                break;

            case Handler::Mode::COMMAND:
                _printer.write(std::cout, "Enter command or to change mode enter '=':");
                std::getline(std::cin, input);

                _handler.appendCommand(input, _robotData);

                if (_handler.getCurrentState() == Handler::State::COORDINATE_TYPE)
                {
                    sendCoordinateSystem(_handler.getCoordinateSystem());
                }
                else if (_handler.getCurrentState() == Handler::State::CIRCLIC)
                {
                    sendCoordinates(_trajectoryManager.circlicMovement(_handler.getParsedResult()));
                }
                else if (_handler.getCurrentState() == Handler::State::POSITIONAL)
                {
                    sendCoordinates(_trajectoryManager.positionalMovement(
                                    _handler.getParsedResult()));
                }
                else if (_handler.getCurrentState() == Handler::State::PARTIAL)
                {
                    sendCoordinates(_trajectoryManager.partialMovement(_handler.getParsedResult()));
                }
                else if (_handler.getCurrentState() == Handler::State::HOME)
                {
                    sendCoordinates(RobotData::getDefaultPosition());
                }
                else if (_handler.getCurrentState() == Handler::State::FROM_FILE)
                {
                    _robotData = _logger.readLine<RobotData>();
                    if (!_logger.hasAnyInputErrors())
                    {
                        sendCoordinates(_robotData);
                    }
                    else
                    {
                        _printer.writeLine(std::cout, "ERROR 05: Some error occurred in input "
                                           "stream! Input stream will be restarted.");
                        _logger.restartStream(logger::Logger::TypeStream::INPUT_STREAM);
                    }
                }
                else if (_handler.getCurrentState() != Handler::State::DEFAULT)
                {
                    sendCoordinates(_robotData);
                }
                else if (input == "test1")
                {
                    _robotData = RobotData::getDefaultPosition();
                    constexpr int kNumberOfIterations = 2'000;
                    constexpr int kDefaultMultiplier  = 100;
                    for (int i = 1; i <= kNumberOfIterations; ++i)
                    {
                        if (i % 2 == 0)
                        {
                            _robotData.coordinates.at(Handler::Y) = kDefaultMultiplier * ((i + 1) / 2);
                        }
                        else
                        {
                            _robotData.coordinates.at(Handler::Y) = -kDefaultMultiplier * ((i + 1) / 2);
                        }

                        sendCoordinates(_robotData);
                        while (!_isReceive.load())
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        }
                        _isReceive.store(false);
                    }
                }
                else if (input == "test2")
                {
                    _robotData = RobotData::getDefaultPosition();
                    constexpr int kNumberOfIterations = 450;
                    constexpr int kDefaultMultiplier = 100;
                    _robotData.coordinates.at(Handler::W) += kDefaultMultiplier * kNumberOfIterations;
                    sendCoordinates(_robotData);
                    int cache = 0;
                    for (int i = 1; i <= kNumberOfIterations; ++i)
                    {
                        if (i % 2 == 0)
                        {
                            const int temp = kDefaultMultiplier * ((i + 1) / 2);
                            _robotData.coordinates.at(Handler::W) += temp - cache;
                            cache = temp;
                        }
                        else
                        {
                            const int temp = -kDefaultMultiplier * ((i + 1) / 2);
                            _robotData.coordinates.at(Handler::W) += temp - cache;
                            cache = temp;
                        }

                        sendCoordinates(_robotData);
                        while (!_isReceive.load())
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        }
                        _isReceive.store(false);
                    }
                }
                else if (input == "test3")
                {
                    _robotData = RobotData::getDefaultPosition();
                    constexpr int kNumberOfIterations = 450;
                    constexpr int kDefaultMultiplier = 100;
                    _robotData.coordinates.at(Handler::P) -= kDefaultMultiplier * kNumberOfIterations;
                    sendCoordinates(_robotData);
                    int cache = 0;
                    for (int i = 1; i <= kNumberOfIterations; ++i)
                    {
                        if (i % 2 == 0)
                        {
                            const int temp = kDefaultMultiplier * ((i + 1) / 2);
                            _robotData.coordinates.at(Handler::P) += temp - cache;
                            cache = temp;
                        }
                        else
                        {
                            const int temp = -kDefaultMultiplier * ((i + 1) / 2);
                            _robotData.coordinates.at(Handler::P) += temp - cache;
                            cache = temp;
                        }

                        sendCoordinates(_robotData);
                        while (!_isReceive.load())
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        }
                        _isReceive.store(false);
                    }
                }
                else if (input == "test4")
                {
                    _robotData = RobotData::getDefaultPosition();
                    constexpr int kNumberOfIterations = 450;
                    constexpr int kDefaultMultiplier = 100;
                    _robotData.coordinates.at(Handler::R) += kDefaultMultiplier * kNumberOfIterations;
                    sendCoordinates(_robotData);
                    int cache = 0;
                    for (int i = 1; i <= kNumberOfIterations; ++i)
                    {
                        if (i % 2 == 0)
                        {
                            const int temp = kDefaultMultiplier * ((i + 1) / 2);
                            _robotData.coordinates.at(Handler::R) += temp - cache;
                            cache = temp;
                        }
                        else
                        {
                            const int temp = -kDefaultMultiplier * ((i + 1) / 2);
                            _robotData.coordinates.at(Handler::R) += temp - cache;
                            cache = temp;
                        }

                        sendCoordinates(_robotData);
                        while (!_isReceive.load())
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        }
                        _isReceive.store(false);
                    }
                }
                break;

            default:
                assert(false);
                break;
        }
    }
}

std::chrono::duration<double> Client::getDuration() const noexcept
{
    return _duration;
}

RobotData Client::getRobotData() const noexcept
{
    return _robotData;
}

bool Client::tryConnect(const int port, const std::string& ip, QTcpSocket* const socketToConnect,
                        const bool isNeedToSendSystem, const int msecs) const
{
    if (socketToConnect->isOpen())
    {
        socketToConnect->close();
    }

    socketToConnect->connectToHost(ip.c_str(), port);

    if (socketToConnect->waitForConnected(msecs))
    {
        _printer.writeLine(std::cout, "Connected to Server!");
        if (isNeedToSendSystem)
        {
            sendCoordinateSystem(CoordinateSystem::WORLD);
        }
        
        return true;
    }

    _printer.writeLine(std::cout, "Not connected to Server!");
    return false;
}

void Client::tryReconnect()
{
    launch();
}

void Client::launch()
{
    bool isConnected;
    switch (_workMode)
    {
        case WorkMode::STRAIGHTFORWARD:
            // Remember that sending socket should be connect to server receiving port
            // and receiving socket should be connect to server sending port!
            isConnected = tryConnect(_serverReceivingPort, _serverIP, _sendingSocket.get(), true)
                       && tryConnect(_serverSendingPort, _serverIP, _receivingSocket.get());
            while (!isConnected)
            {
                isConnected = tryConnect(_serverReceivingPort, _serverIP, _sendingSocket.get(), true)
                           && tryConnect(_serverSendingPort, _serverIP, _receivingSocket.get());
                
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(CONFIG.get<Param::RECONNECTION_DELAY>()));
            }
            break;

        case WorkMode::INDIRECT:
            isConnected = tryConnect(_layerPort, _serverIP, _socketForLayer.get(), true);
            while (!isConnected)
            {
                isConnected = tryConnect(_layerPort, _serverIP, _socketForLayer.get(), true);
                
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(CONFIG.get<Param::RECONNECTION_DELAY>()));
            }
            break;

        default:
            assert(false);
            break;
    }

    _printer.writeLine(std::cout, "\nClient launched...\n");
    _logger.writeLine("\nClient launched at", utils::getCurrentSystemTime());
}

void Client::sendData(const std::string& data) const
{
    emit signalToSend(data.c_str());
}

void Client::sendCoordinates(const RobotData& robotData)
{
    _start = std::chrono::steady_clock::now();
    sendData(robotData.toString());
    _robotData = robotData;
    _logger.writeLine(robotData);
}

void Client::sendCoordinates(const std::vector<RobotData>& points)
{
    for (const auto& point : points)
    {
        sendCoordinates(point);
    }
}

void Client::sendCoordinateSystem(const CoordinateSystem coordinateSystem) const
{
    switch (coordinateSystem)
    {
        case CoordinateSystem::JOINT:
            sendData("0");
            break;

        case CoordinateSystem::JGFRM:
            _printer.writeLine(std::cout, "Not supported yet!");
            break;
        
        case CoordinateSystem::WORLD:
            sendData("2");
            break;

        case CoordinateSystem::INVALID:
            [[fallthrough]];
        default:
            _printer.writeLine(std::cout, "ERROR 04: Incorrect coordinate system to send!");
            break;
    }
}

} // namespace vasily
