#include <iostream>
#include <cassert>

#include "Client.h"


namespace vasily
{

Client::Client(const int serverPort, const std::string_view serverIP)
	: WinsockInterface(),
	  _robotData(),
	  _serverIP(serverIP),
	  _serverPort(serverPort),
	  _serverPortSending(0),
	  _serverPortReceiving(0),
	  _handler(),
	  _start(std::chrono::steady_clock::now()),
	  _duration(),
	  _waitAnswer(),
	  _isNeedToWait(false),
	  _circlicState()
{
}

Client::Client(const int serverPortSending, const int serverReceiving,
			   const std::string_view serverIP)
	: WinsockInterface(),
	  _robotData(),
	  _serverIP(serverIP),
	  _serverPort(0),
	  _serverPortSending(serverPortSending),
	  _serverPortReceiving(serverReceiving),
	  _handler(),
	  _start(std::chrono::steady_clock::now()),
	  _duration(),
	  _waitAnswer(),
	  _isNeedToWait(false),
	  _circlicState()
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
	  _circlicState(other._circlicState)
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
	utils::println("\n\n\nThread started...\n");

	std::string dataBuffer;
	std::size_t count = 0u;

	while (true)
	{
		dataBuffer = receiveData(_socketReceive);

		if (!_isRunning)
		{
			tryReconnect();
		}

		if (_isNeedToWait)
		{
			RobotData robotData;
			std::vector<std::string> strStorage;
			utils::split(dataBuffer, strStorage);
				
			std::size_t countForCoords = 0u;
			for (const auto& str : strStorage)
			{
				if (utils::isCorrectNumber(str))
				{
					robotData.mCoordinates.at(countForCoords) = utils::stringToInt(str);
					++countForCoords;

					if (countForCoords > 5u)
					{
						break;
					}
				}
			}

			// NEED TO DO AFTER DANILA REFACTORING.
			///if (robotData == _waitAnswer)
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

		++count;
		_duration = std::chrono::steady_clock::now() - _start;
		utils::println(count, "Duration:", _duration.count(), "seconds");
	}
}

void Client::checkConnection(const std::atomic_int64_t& time)
{
	constexpr std::atomic_int64_t waitingTime = 1000LL;
	std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));

	while (true)
	{
		sendCoordinates(_robotData);
	
		std::this_thread::sleep_for(std::chrono::milliseconds(time));
	}
}

void Client::waitLoop()
{
	std::string input;

	// Create data for robot.
	_robotData = _DEFAULT_POSITION;

	sendData(_socketSend, "2");

	std::thread reciveThread(&Client::receive, this);
	reciveThread.detach();

	constexpr std::atomic_int64_t waitingTime = 25LL;
	std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));

	// NEED TO DO AFTER DANILA REFACTORING.
	///constexpr std::atomic_int64_t time = 2000LL;
	///std::thread checkThread(&Client::checkConnection, this, std::cref(time));
	///checkThread.detach();

	utils::println("\n\n\nWaiting for reply...\n");

	while (true)
	{
		memset(_message, 0, _MAXRECV);

		switch (_handler.getCurrentMode())
		{
			case Handler::Mode::READING:
				utils::print("Enter coordinates or to change mode enter '=':");
				std::getline(std::cin, input);

				_handler.parseRawData(input, _robotData);

				if (_handler.getCurrentState() == Handler::State::COORDINATE_TYPE)
				{
					switch (_handler.getCoordinateSystem())
					{
						case Handler::CoorninateSystem::JOINT:
							sendData(_socketSend, "1");
							break;

						case Handler::CoorninateSystem::WORLD:
							sendData(_socketSend, "2");
							break;

						default:
							assert(false);
							break;
					}
				}
				else if (_handler.getCurrentState() == Handler::State::FULL_CONTROL)
				{
					sendCoordinates(_robotData);
				}
				break;

			case Handler::Mode::COMMAND:
				utils::print("Enter command or to change mode enter '=':");
				std::getline(std::cin, input);

				_handler.appendCommand(input, _robotData);

				if (_handler.getCurrentState() == Handler::State::COORDINATE_TYPE)
				{
					switch (_handler.getCoordinateSystem())
					{
						case Handler::CoorninateSystem::JOINT:
							sendData(_socketSend, "1");
							break;

						case Handler::CoorninateSystem::WORLD:
							sendData(_socketSend, "2");
							break;
						
						default: 
							assert(false);
							break;
					}
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
				else if (_handler.getCurrentState() != Handler::State::DEFAULT)
				{
					sendCoordinates(_robotData);
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

// NEED TO CHANGE THIS FUNCTION AFTER DANILA REFACTORING.
void Client::tryReconnect()
{
	while (!_isRunning)
	{
		closesocket(_socketSend);
		closesocket(_socketReceive);

		initSocket(_socketSend);
		initSocket(_socketReceive);

		///_isRunning = tryConnect(_serverPort, _serverIP, _socketSend, _socketSendAddress);
		_isRunning = tryConnect(_serverPortSending, _serverIP, _socketSend, _socketSendAddress)
					&& tryConnect(_serverPortReceiving, _serverIP, _socketReceive,
								  _socketReceiveAddress);

		constexpr std::atomic_int64_t waitingTime = 1000LL;
		std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
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
	tryConnect(_serverPortSending, _serverIP, _socketSend, _socketSendAddress);
	tryConnect(_serverPortReceiving, _serverIP, _socketReceive, _socketReceiveAddress);
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
				constexpr std::atomic_int64_t waitingTime = 10LL;
				std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
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
				constexpr std::atomic_int64_t waitingTime = 10LL;
				std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
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

		constexpr std::atomic_int64_t waitingTime = 25LL;
		std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));

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
		if (robotData.mCoordinates.at(i) < _MIN_COORDS.at(i))
		{
			return false;
		}

		if (robotData.mCoordinates.at(i) > _MAX_COORDS.at(i))
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
		_start = std::chrono::steady_clock::now();
		sendData(_socketSend, robotData.toString());
		return true;
	}
	
	utils::println("ERROR 04: Incorrect coordinates to send!", robotData.toString());
	return false;
}

void Client::sendCoordinateType(const CoordinateType coordinateType) const
{
	switch (coordinateType)
	{
		case CoordinateType::JOINT:
			sendData(_socketSend, "1");
			break;
		
		case CoordinateType::WORLD:
			sendData(_socketSend, "2");
			break;
		
		default:
			utils::println("ERROR 05: Incorrect coordinate system to send!");
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
