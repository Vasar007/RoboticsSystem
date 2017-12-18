#include <cassert>
#include <thread>

#include "Client.h"


namespace vasily
{

Client::Client(const int serverPort, const std::string_view serverIP)
	: WinsockInterface(),
	  _robotData(),
	  _serverIP(serverIP),
	  _serverPort(serverPort),
	  _serverSendingPort(0),
	  _serverReceivingPort(0),
	  _handler(),
	  _start(std::chrono::steady_clock::now()),
	  _duration(),
	  _waitAnswer(),
	  _isNeedToWait(false),
	  _circlicState(),
	  _logger(_DEFAULT_IN_FILE_NAME, _DEFAULT_OUT_FILE_NAME),
	  isNeedToUpdate(false)
{
}

Client::Client(const int serverPortSending, const int serverReceiving,
			   const std::string_view serverIP)
	: WinsockInterface(),
	  _robotData(),
	  _serverIP(serverIP),
	  _serverPort(0),
	  _serverSendingPort(serverPortSending),
	  _serverReceivingPort(serverReceiving),
	  _handler(),
	  _start(std::chrono::steady_clock::now()),
	  _duration(),
	  _waitAnswer(),
	  _isNeedToWait(false),
	  _circlicState(),
	  _logger(_DEFAULT_IN_FILE_NAME, _DEFAULT_OUT_FILE_NAME),
	  isNeedToUpdate(false)
{
}

Client::Client(Client&& other) noexcept
	: WinsockInterface(),
	  _robotData(other._robotData),
	  _serverIP(other._serverIP),
	  _serverPort(other._serverPort),
	  _handler(other._handler),
	  _start(other._start),
	  _waitAnswer(other._waitAnswer),
	  _isNeedToWait(other._isNeedToWait ? true : false),
	  _circlicState(other._circlicState),
	  _logger(_DEFAULT_IN_FILE_NAME, _DEFAULT_OUT_FILE_NAME),
	  isNeedToUpdate(false)
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
	utils::println(std::cout, "\n\n\nThread started...\n");
	_logger.write("\n\nClient launched at", utils::getCurrentSystemTime());

	std::size_t count = 0u;
	while (true)
	{
		std::string dataBuffer = receiveData(_receivingSocket);

		if (!_isRunning)
		{
			tryReconnect();
			continue;
		}

		if (_isNeedToWait)
		{
			// NEED TO DO AFTER DANILA REFACTORING.
			///RobotData robotData;
			///bool flag;
			///robotData = utils::fromString<RobotData>(dataBuffer, flag);

			///if (flag && robotData == _waitAnswer)
			{
				_isNeedToWait = false;
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

		isNeedToUpdate = true;

		++count;
		_duration = std::chrono::steady_clock::now() - _start;
		utils::println(std::cout, count, "Duration:", _duration.count(), "seconds");

		_logger.write(_message, '-', dataBuffer);
		_logger.write(count, "Duration:", _duration.count(), "seconds");
	}
}

void Client::checkConnection(const std::atomic_int64_t& time)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1000LL));

	while (true)
	{
		sendCoordinates(_robotData);
	
		std::this_thread::sleep_for(std::chrono::milliseconds(time));
	}
}

void Client::waitLoop()
{
	// Create data for robot.
	_robotData = _DEFAULT_POSITION;

	sendData(_sendingSocket, "2");

	std::thread reciveThread(&Client::receive, this);
	reciveThread.detach();

	std::this_thread::sleep_for(std::chrono::milliseconds(25LL));

	// NEED TO DO AFTER DANILA REFACTORING.
	///constexpr std::atomic_int64_t time = 2000LL;
	///std::thread checkThread(&Client::checkConnection, this, std::cref(time));
	///checkThread.detach();

	utils::println(std::cout, "\n\n\nWaiting for reply...\n");

	std::string input;
	while (true)
	{
		memset(_message, 0, _MAXRECV);

		switch (_handler.getCurrentMode())
		{
			case Handler::Mode::READING:
				utils::print(std::cout, "Enter coordinates or to change mode enter '=':");
				std::getline(std::cin, input);

				_handler.parseRawData(input, _robotData);

				if (_handler.getCurrentState() == Handler::State::COORDINATE_TYPE)
				{
					sendCoordinateType(_handler.getCoordinateSystem());
				}
				else if (_handler.getCurrentState() == Handler::State::FULL_CONTROL)
				{
					sendCoordinates(_robotData);
				}
				break;

			case Handler::Mode::COMMAND:
				utils::print(std::cout, "Enter command or to change mode enter '=':");
				std::getline(std::cin, input);

				_handler.appendCommand(input, _robotData);

				if (_handler.getCurrentState() == Handler::State::COORDINATE_TYPE)
				{
					sendCoordinateType(_handler.getCoordinateSystem());
				}
				else if (_handler.getCurrentState() == Handler::State::CIRCLIC)
				{
					const ParsedResult parsedResult = _handler.getParsedResult();
					circlicMovement(parsedResult.mFirstPoint, parsedResult.mSecondPoint,
									parsedResult.mNumberOfIterations);
				}
				else if (_handler.getCurrentState() == Handler::State::PARTIAL)
				{
					const ParsedResult parsedResult = _handler.getParsedResult();
					partialMovement(parsedResult.mFirstPoint, parsedResult.mSecondPoint,
									parsedResult.mNumberOfIterations);
				}
				else if (_handler.getCurrentState() == Handler::State::HOME)
				{
					sendCoordinates(_DEFAULT_POSITION);
				}
				else if (_handler.getCurrentState() == Handler::State::FROM_FILE)
				{
					_robotData = _logger.read<RobotData>();
					if (!_logger.hasAnyInputErrors())
					{
						sendCoordinates(_robotData);
					}
					else
					{
						utils::println(std::cout, "ERROR 06: Some error occurred in input stream!",
									   "Input stream will be restarted.");
						_logger.restartStream(logger::Logger::TypeStream::INPUT_STREAM);
					}
				}
				else if (_handler.getCurrentState() != Handler::State::DEFAULT)
				{
					sendCoordinates(_robotData);
				}
				else if (input == "test")
				{
					_robotData = _DEFAULT_POSITION;
					_robotData.coordinates.at(Handler::Y) = -400'000;
					std::string toSending = _robotData.toString();

					_robotData.coordinates.at(Handler::Y) = 100'000;
					toSending += _robotData.toString();
					for (std::size_t i = 0u; i < 10u; ++i)
					{
						_robotData.coordinates.at(Handler::Y) += 20'000;
						if (checkCoordinates(_robotData))
						{
							toSending += _robotData.toString();
						}
					}

					_logger.write(toSending);
					_start = std::chrono::steady_clock::now();
					sendData(_sendingSocket, toSending);
				}
				break;
			
			default:
				assert(false);
				break;
		}
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

// NEED TO CHANGE THIS FUNCTION AFTER DANILA REFACTORING.
void Client::tryReconnect()
{
	while (!_isRunning)
	{
		closesocket(_sendingSocket);
		closesocket(_receivingSocket);

		initSocket(_sendingSocket);
		initSocket(_receivingSocket);

		///_isRunning = tryConnect(_serverPort, _serverIP, _socketSend, _socketSendAddress);
		_isRunning = tryConnect(_serverSendingPort, _serverIP, _sendingSocket, _sendingSocketAddress)
					&& tryConnect(_serverReceivingPort, _serverIP, _receivingSocket,
								  _receivingSocketAddress);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000LL));
	}
}

void Client::run()
{
	_isRunning = true;
	waitLoop();
}

void Client::launch()
{
	// NEED TO SWAP THIS CODE AFTER DANILA REFACTORING.
	///tryConnect(_serverPort, _serverIP, _socketSend, _socketSendAddress);
	///setTimeout(_socketSend, 1000, 0);
	tryConnect(_serverSendingPort, _serverIP, _sendingSocket, _sendingSocketAddress);
	tryConnect(_serverReceivingPort, _serverIP, _receivingSocket, _receivingSocketAddress);
}

void Client::circlicProcessing(const RobotData& firstPoint, const RobotData& secondPoint, 
							   const std::size_t numberOfIterations)
{
	_circlicState = CirclicState::SEND_FIRST;

	std::size_t counterIterations = 0u;

	bool flag = true;

	while (flag)
	{
		switch (_circlicState)
		{
			case CirclicState::SEND_FIRST:
				if (counterIterations == numberOfIterations)
				{
					flag = false;
					break;
				}
				++counterIterations;

				_isNeedToWait	= true;
				_waitAnswer		= firstPoint;
				_circlicState	= CirclicState::WAIT_FIRST_ANSWER;

				if (!sendCoordinates(firstPoint))
				{
					return;
				}
				break;

			case CirclicState::WAIT_FIRST_ANSWER:
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10LL));
				break;
			}

			case CirclicState::SEND_SECOND:
				_isNeedToWait	= true;
				_waitAnswer		= secondPoint;
				_circlicState	= CirclicState::WAIT_SECOND_ANSWER;

				if (!sendCoordinates(secondPoint))
				{
					return;
				}
				break;

			case CirclicState::WAIT_SECOND_ANSWER:
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10LL));
				break;
			}

			default:
				assert(false);
				break;
		}
	}

	sendCoordinates(firstPoint);
}

void Client::partialProcessing(const RobotData& firstPoint, const RobotData& secondPoint,
							   const std::size_t numberOfSteps)
{
	const RobotData directionalVector	= (secondPoint - firstPoint) / numberOfSteps;
	RobotData robotData					= firstPoint;

	if (!sendCoordinates(firstPoint))
	{
		return;
	}

	for (std::size_t i = 0u; i < numberOfSteps; ++i)
	{
		robotData += directionalVector;

		std::this_thread::sleep_for(std::chrono::milliseconds(25LL));

		if (!sendCoordinates(robotData))
		{
			return;
		}
	}

	if (robotData != secondPoint)
	{
		sendCoordinates(secondPoint);
	}
}

bool Client::checkCoordinates(const RobotData& robotData) const
{
	for (std::size_t i = 0u; i < _MAIN_COORDINATES; ++i)
	{
		if (robotData.coordinates.at(i) < _MIN_COORDS.at(i))
		{
			return false;
		}

		if (robotData.coordinates.at(i) > _MAX_COORDS.at(i))
		{
			return false;
		}
	}

	return true;
}

bool Client::sendCoordinates(const RobotData& robotData)
{
	if (checkCoordinates(robotData))
	{
		_robotData = robotData;
		_logger.write(robotData.toString());
		_start = std::chrono::steady_clock::now();
		sendData(_sendingSocket, robotData.toString());
		return true;
	}
	
	utils::println(std::cout, "ERROR 04: Incorrect coordinates to send!", robotData.toString());
	return false;
}

void Client::sendCoordinateType(const CoordinateSystem coordinateType) const
{
	switch (coordinateType)
	{
		case CoordinateSystem::JOINT:
			sendData(_sendingSocket, "1");
			break;
		
		case CoordinateSystem::WORLD:
			sendData(_sendingSocket, "2");
			break;
		
		default:
			utils::println(std::cout, "ERROR 05: Incorrect coordinate system to send!");
			break;
	}
}

void Client::circlicMovement(const RobotData& firstPoint, const RobotData& secondPoint, 
							 const std::size_t numberOfIterations)
{
	std::thread circlicThread(&Client::circlicProcessing, this, std::cref(firstPoint), 
							  std::cref(secondPoint), numberOfIterations);
	circlicThread.join();
}

void Client::partialMovement(const RobotData& firstPoint, const RobotData& secondPoint,
							 const std::size_t numberOfSteps)
{
	std::thread partialThread(&Client::partialProcessing, this, std::cref(firstPoint), 
							  std::cref(secondPoint), numberOfSteps);
	partialThread.join();
}

}
