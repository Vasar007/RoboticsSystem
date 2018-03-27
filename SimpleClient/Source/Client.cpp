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
    { "192.168.1.21", 13 },
     59002,
     59003,
     1000LL
};
 
Client::Client(const int serverPort, const std::string_view serverIP, const WorkMode workMode)
    : isNeedToUpdate(false),
      lastSentPoint(RobotData::DEFAULT_CORDINATES, RobotData::DEFAULT_PARAMETERS),
      _serverIP(serverIP),
      _serverPort(serverPort),
      _serverSendingPort(0),
      _serverReceivingPort(0),
      _start(std::chrono::steady_clock::now()),
      _isNeedToWait(false),
      _circlicState(),
      _workMode(workMode),
      _logger(CONFIG.get<CAST(Param::DEFAULT_IN_FILE_NAME)>(),
              CONFIG.get<CAST(Param::DEFAULT_OUT_FILE_NAME)>())
{
}

Client::Client(const int serverSendingPort, const int serverReceivingPort,
               const std::string_view serverIP, const WorkMode workMode)
    : isNeedToUpdate(false),
      lastSentPoint(RobotData::DEFAULT_CORDINATES, RobotData::DEFAULT_PARAMETERS),
      _serverIP(serverIP),
      _serverPort(0),
      _serverSendingPort(serverSendingPort),
      _serverReceivingPort(serverReceivingPort),
      _start(std::chrono::steady_clock::now()),
      _isNeedToWait(false),
      _circlicState(),
      _workMode(workMode),
      _logger(CONFIG.get<CAST(Param::DEFAULT_IN_FILE_NAME)>(),
              CONFIG.get<CAST(Param::DEFAULT_OUT_FILE_NAME)>())
{
}

Client::Client(Client&& other) noexcept
    : isNeedToUpdate(false),
      _robotData(other._robotData),
      _serverIP(std::move(other._serverIP)),
      _serverPort(other._serverPort),
      _serverSendingPort(other._serverSendingPort),
      _serverReceivingPort(other._serverReceivingPort),
      _handler(std::move(other._handler)),
      _start(other._start),
      _waitAnswer(other._waitAnswer),
      _isNeedToWait(other._isNeedToWait.load()),
      _circlicState(other._circlicState),
      _workMode(other._workMode),
      _logger(CONFIG.get<CAST(Param::DEFAULT_IN_FILE_NAME)>(),
              CONFIG.get<CAST(Param::DEFAULT_OUT_FILE_NAME)>())
{
    utils::swap(*this, other);
}

Client& Client::operator=(Client&& other) noexcept
{
    if (this != &other)
    {
        utils::swap(*this, other);
    }
    return *this;
}

void Client::receive()
{
    _printer.writeLine(std::cout, "\n\n\nThread started...\n");
    _logger.writeLine("\n\nClient launched at", utils::getCurrentSystemTime());

    int count = 0;
    while (true)
    {
        std::string dataBuffer;
        switch (_workMode)
        {
            case WorkMode::INDIRECT:
            {
                auto [message, flag] = receiveData(_sendingSocket, _messageWithIP, _buffer);
                dataBuffer = std::move(message);
                _isRunning.store(flag);
                break;
            }
            
            case WorkMode::STRAIGHTFORWARD:
            {
                auto [message, flag] = receiveData(_receivingSocket, _messageWithIP, _buffer);
                dataBuffer = std::move(message);
                _isRunning.store(flag);
                break;
            }
            
            default:
                assert(false);
                break;
        }
         _duration = std::chrono::steady_clock::now() - _start;
         _start = std::chrono::steady_clock::now();

        if (!_isRunning.load())
        {
            tryReconnect();
            continue;
        }

        if (_isNeedToWait.load())
        {
            ///RobotData robotData; => Danila works on new movement algorithms.
            ///bool flag;
            ///robotData = utils::fromString<RobotData>(dataBuffer, flag);

            ///if (flag && robotData == _waitAnswer)
            {
                _isNeedToWait.store(false);
                switch (_circlicState)
                {
                    case CirclicState::SEND_FIRST:
                        break;
                        
                    case CirclicState::WAIT_FIRST_ANSWER:
                        _circlicState = CirclicState::SEND_SECOND;
                        break;
                        
                    case CirclicState::SEND_SECOND:
                        break;
                        
                    case CirclicState::WAIT_SECOND_ANSWER:
                        _circlicState = CirclicState::SEND_FIRST;
                        break;
                        
                    default:
                        break;
                }
            }
        }

        isNeedToUpdate.store(true);
        ++count;
        _printer.writeLine(std::cout, count, "Duration:", _duration.count(), "seconds");

        _logger.writeLine(_messageWithIP, '-', dataBuffer);
        _logger.writeLine(count, "Duration:", _duration.count(), "seconds");
    }
}

void Client::checkConnection(const long long time)
{
    while (true)
    {
        sendCoordinates(_robotData);
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }
}

void Client::waitLoop()
{
    sendCoordinateSystem(CoordinateSystem::JOINT);

    std::thread reciveThread(&Client::receive, this);
    reciveThread.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(25LL));

    ///constexpr long long kTime = 2000LL;
    ///std::thread checkThread(&Client::checkConnection, this, std::cref(kTime));
    ///checkThread.detach();

    _printer.writeLine(std::cout, "\n\n\nWaiting for reply...\n");

    while (true)
    {
        memset(_messageWithIP, 0, _MAXRECV);

        std::string input;
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
                    sendCoordinates(_trajectoryManager.CirclicMovement(_handler.getParsedResult()));
				}
                else if (_handler.getCurrentState() == Handler::State::POSITIONAL)
                {
                    sendCoordinates(_trajectoryManager.PositionalMovement(
                        _handler.getParsedResult()));
                }
                else if (_handler.getCurrentState() == Handler::State::PARTIAL)
				{
                    sendCoordinates(_trajectoryManager.PartialMovement(_handler.getParsedResult()));
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
				else if (input == "test")
				{
					_robotData = RobotData::getDefaultPosition();
					constexpr int kNumberOfIterations = 1000;
					constexpr int kDefaultMultiplier  = 100;
					for (int i = 1; i <= kNumberOfIterations; ++i)
					{
						_robotData.coordinates.at(Handler::Y) += kDefaultMultiplier * i
																* (i & 1 ? 1 : -1);
						sendCoordinates(_robotData);
						_isNeedToWait.store(true);
						while (_isNeedToWait.load())
						{
							std::this_thread::sleep_for(std::chrono::milliseconds(1LL));
						}
					}
				}
				else if (input == "tenzo")
				{
					tenzoCalibration();
					//workWithTenzo();
				}
				else if (input == "tzTest")
				{
					StrainGauge tenzo(L"COM13");
					while (true)
					{
						std::array<double, 6> tmp = tenzo.readComStrain();
						std::array<double, 6> collectedData{ tmp[1], -tmp[0], -tmp[2],
															 tmp[4], -tmp[3],  tmp[5] };
						std::copy(collectedData.begin(), collectedData.end(),
								  std::ostream_iterator<double>(std::cout, "\t"));
					}
				}
				break;
			
			default:
				assert(false);
				break;
		}
	}
}

void Client::launch()
{
    switch (_workMode)
    {
        case WorkMode::STRAIGHTFORWARD:
        {
            const bool connectStatus = tryConnect(_serverSendingPort, _serverIP, _sendingSocket, 
                                                  _sendingSocketAddress)
                                    && tryConnect(_serverReceivingPort, _serverIP,
                                                  _receivingSocket, _receivingSocketAddress);
            _isRunning.store(connectStatus);
            break;
        }

        case WorkMode::INDIRECT:
        {
            const bool connectStatus = tryConnect(_serverPort, _serverIP, _sendingSocket,
                                                  _sendingSocketAddress);
            _isRunning.store(connectStatus);
            break;
        }

        default:
            assert(false);
            break;
    }
}

std::string Client::getServerIP() const
{
    return _serverIP;
}

void Client::setServerIP(const std::string_view newServerIP)
{
    _serverIP = newServerIP;
}

std::chrono::duration<double> Client::getDuration() const
{
    return _duration;
}

RobotData Client::getRobotData() const
{
    return _robotData;
}

void Client::tryReconnect()
{
    while (!_isRunning.load())
    {
        closeSocket(_sendingSocket);
        initSocket(_sendingSocket);
        switch (_workMode)
        {
            case WorkMode::STRAIGHTFORWARD:
                closeSocket(_receivingSocket);
                initSocket(_receivingSocket);
                break;
            
            case WorkMode::INDIRECT:
                break;
            
            default:
                assert(false);
                break;
        }
        launch();

        std::this_thread::sleep_for(
            std::chrono::milliseconds(CONFIG.get<CAST(Param::RECONNECTION_DELAY)>()));
    }
}

void Client::run()
{
    _isRunning.store(true);
    waitLoop();
}

void Client::sendCoordinates(const RobotData& robotData)
{
    _start          = std::chrono::steady_clock::now();
    sendData(_sendingSocket, robotData.toString());
    lastSentPoint   = robotData;
    _robotData      = robotData;
    _logger.writeLine(robotData);
}

void Client::sendCoordinates(const std::vector<RobotData>& points)
{
    _start = std::chrono::steady_clock::now();
    for(auto& it:points)
    {
        sendData(_sendingSocket, it.toString());
        _logger.writeLine(it);
    }
    _robotData = *points.rbegin();
    lastSentPoint = _robotData;
}

void Client::sendCoordinateSystem(const CoordinateSystem coordinateSystem) const
{
    switch (coordinateSystem)
    {
        case CoordinateSystem::JOINT:
            sendData(_sendingSocket, "0");
            break;

        case CoordinateSystem::JGFRM:
            _printer.writeLine(std::cout, "Not supported yet!");
            break;
        
        case CoordinateSystem::WORLD:
            sendData(_sendingSocket, "2");
            break;

        case CoordinateSystem::INVALID:
            [[fallthrough]];
        default:
            _printer.writeLine(std::cout, "ERROR 04: Incorrect coordinate system to send!");
            break;
    }
}

void Client::tenzoCalibration()
{
    constexpr std::size_t kNumberOfPositions = 6u;
    for (std::size_t index = 0u; index < kNumberOfPositions; ++index)
    {
        const RobotData robotData(_tenzoMath.getPosition(index));
        sendCoordinates(robotData);

        _tenzoMath.collectData(index);
    }
    _tenzoMath.doCalibration();

    // Robot start position.
    const RobotData startPos({ 0, 0, 0, 0, -90'000, 0 });
    sendCoordinates(startPos);
}

void Client::workWithTenzo()
{
    _tenzoMath.loadCalibData();

    RobotData robotData{};
    while (true)
    {
        _tenzoMath.calculatePos(robotData.coordinates);
        sendCoordinates(robotData);
    }
}

} // namespace vasily
