#include <thread>
#include <iostream>

#include "RobotImitator.h"


namespace vasily
{

inline const config::Config<std::string, std::string> RobotImitator::CONFIG
{
    { "in.txt" },
    { "out.txt" }
};

RobotImitator::RobotImitator(const int recivingPort, const int sendingPort,
                             QObject* parent)
    : QObject(parent),
      _receivingPort(recivingPort),
      _receivingSocket(std::make_unique<QTcpServer>(this)),
      _clientSendingSocket(nullptr),
      _sendingPort(sendingPort),
      _sendingSocket(std::make_unique<QTcpServer>(this)),
      _clientReceivingSocket(nullptr),
      _logger(CONFIG.get<CAST(Param::DEFAULT_IN_FILE_NAME)>(),
              CONFIG.get<CAST(Param::DEFAULT_OUT_FILE_NAME)>())
{
    _printer.writeLine(std::cout, "Receiving Port:", recivingPort, "Sending Port:", sendingPort);

    connect(_receivingSocket.get(), &QTcpServer::newConnection, this,
            &RobotImitator::slotNewConnectionOnReceive);
    connect(_sendingSocket.get(), &QTcpServer::newConnection, this,
            &RobotImitator::slotNewConnectionOnSend);
}

void RobotImitator::slotNewConnectionOnReceive()
{
    _printer.writeLine(std::cout, "\nNew connection to receiving port\n");
    _logger.writeLine("\nNew connection to receiving port at",
                      utils::getCurrentSystemTime());

    _clientSendingSocket = _receivingSocket->nextPendingConnection();

    connect(_clientSendingSocket, &QTcpSocket::readyRead, this, &RobotImitator::slotReadFromClient);
    connect(_clientSendingSocket, &QTcpSocket::disconnected, this,
            &RobotImitator::slotClientDisconnectedOnReceive);
}

void RobotImitator::slotNewConnectionOnSend()
{
    _printer.writeLine(std::cout, "\nNew connection to sending port\n");
    _logger.writeLine("\nNew connection to sending port at",
                      utils::getCurrentSystemTime());

    _clientReceivingSocket = _sendingSocket->nextPendingConnection();

    _clientReceivingSocket->write("Test message from Imitator::SendingSocket.");

    connect(_clientReceivingSocket, &QTcpSocket::disconnected, this,
            &RobotImitator::slotClientDisconnectedOnSend);
}

void RobotImitator::slotReadFromClient()
{
    if (_clientSendingSocket->bytesAvailable() > 0)
    {
        QByteArray array = _clientSendingSocket->readAll();
        ///qDebug() << array << '\n';

        const std::string receivedData = array.toStdString();

        if (const auto[value, check] = utils::parseCoordinateSystem(receivedData); check)
        {
            const std::string coordSystemStr = receivedData.substr(0, 1);
            _printer.writeLine(std::cout, coordSystemStr);
            _coorninateSystem.emplace(value);
        }

        _logger.writeLine(_clientSendingSocket->localPort(), '-', receivedData); // messageWithIP
        std::string toSending = utils::parseFullData(receivedData);

        bool flag;
        const auto robotData = utils::fromString<RobotData>(receivedData, flag);
        if (flag)
        {
            std::this_thread::sleep_for(calculateDuration(robotData));
        }

        if (!toSending.empty())
        {
            _clientReceivingSocket->write(toSending.c_str());
            _printer.writeLine(std::cout, receivedData);
        }
    }
}

void RobotImitator::slotClientDisconnectedOnReceive()
{
    _printer.writeLine(std::cout, "Client disconnected from receiving port!");
    _clientSendingSocket->close();
    _coorninateSystem.reset();
}

void RobotImitator::slotClientDisconnectedOnSend() const
{
    _printer.writeLine(std::cout, "Client disconnected from sending port!");
    _clientReceivingSocket->close();
}

void RobotImitator::launch() const
{
    if (_receivingSocket->listen(QHostAddress::Any, _receivingPort))
    {
        _printer.writeLine(std::cout, "Receiving server is started!");
    }
    else
    {
        _printer.writeLine(std::cout, "Receiving server is not started!");
    }

    if (_sendingSocket->listen(QHostAddress::Any, _sendingPort))
    {
        _printer.writeLine(std::cout, "Sending server is started!");
    }
    else
    {
        _printer.writeLine(std::cout, "Sending server is not started!");
    }

    _printer.writeLine(std::cout, "\nWaiting for reply...\n");
}

std::chrono::milliseconds RobotImitator::calculateDuration(const RobotData& robotData)
{
    // Calculate distance between two points, which contains only first 3 coordinates.
    const double distance = utils::distance(_lastReceivedData.coordinates.begin(),
                                            _lastReceivedData.coordinates.begin() + 2,
                                            robotData.coordinates.begin(), 0.0, 10'000.0);
    _lastReceivedData = robotData;

    constexpr long long kMultiplier = 65;
    const auto result = static_cast<long long>(distance * kMultiplier);

    return std::chrono::milliseconds(result);
}

} // namespace vasily
