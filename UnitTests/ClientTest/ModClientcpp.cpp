#include "ModClient.h"


namespace clientTests
{

	ModClient::ModClient(const int serverPortSending, const int serverReceiving, 
						 const std::string_view serverIP)
	: 
	Client(serverPortSending, serverReceiving, serverIP),
	mHasFinished(),
	mStorage()
{
}

void ModClient::receiveDataNTimes(const std::size_t numberOfTimes)
{
	std::lock_guard<std::mutex> lockGuard(mMutex);

	for (std::size_t step = 0u; step < numberOfTimes; ++step)
	{
		receiveData(_socketReceive);

		if (_isNeedToWait)
		{
			_isNeedToWait = false;
			switch (_circlicState)
			{
				case ModClient::CirclicState::SEND_FIRST:
					break;

				case ModClient::CirclicState::WAIT_FIRST_ANSWER:
					_circlicState = ModClient::CirclicState::SEND_SECOND;
					break;

				case ModClient::CirclicState::SEND_SECOND:
					break;

				case ModClient::CirclicState::WAIT_SECOND_ANSWER:
					_circlicState = ModClient::CirclicState::SEND_FIRST;
					break;

				default:
					break;;
			}
		}
	}

	mHasFinished = true;
}

void ModClient::sendCoordinatesMod(const vasily::RobotData& robotData)
{
	if (checkCoordinates(robotData))
	{
		mStorage.push_back(robotData.toString());
	}

	sendCoordinates(robotData);
}

void ModClient::circlicMovementMod(const vasily::RobotData& firstPoint,
								   const vasily::RobotData& secondPoint, 
								   const std::size_t numberOfIterations)
{
	const bool checkFirst	= checkCoordinates(firstPoint);
	const bool checkSecond	= checkCoordinates(secondPoint);

	for (std::size_t i = 0u; i < numberOfIterations; ++i)
	{
		if (checkFirst)
		{
			mStorage.push_back(firstPoint.toString());
		}
		else
		{
			break;
		}

		if (checkSecond)
		{
			mStorage.push_back(secondPoint.toString());
		}
		else
		{
			break;
		}
	}

	if (checkFirst && checkSecond)
	{
		mStorage.push_back(firstPoint.toString());
	}
	

	circlicMovement(firstPoint, secondPoint, numberOfIterations);
}

void ModClient::partialMovementMod(const vasily::RobotData& firstPoint,
								   const vasily::RobotData& secondPoint, 
								   const std::size_t numberOfSteps)
{
	const vasily::RobotData directionalVector = (secondPoint - firstPoint) / numberOfSteps;
	vasily::RobotData robotData				  = firstPoint;

	bool isRight = false;

	if (checkCoordinates(firstPoint))
	{
		mStorage.push_back(firstPoint.toString());
		isRight = true;
	}

	for (std::size_t i = 0u; i < numberOfSteps; ++i)
	{
		robotData += directionalVector;

		if (checkCoordinates(robotData) && isRight)
		{
			mStorage.push_back(robotData.toString());
		}
		else
		{
			isRight = false;
			break;
		}
	}

	if (robotData != secondPoint && checkCoordinates(secondPoint) && isRight)
	{
		mStorage.push_back(secondPoint.toString());
	}

	partialMovement(firstPoint, secondPoint, numberOfSteps);
}

std::thread ModClient::spawn(const std::size_t numberOfTimes)
{
	return std::thread(&ModClient::receiveDataNTimes, this, numberOfTimes);
}

}
