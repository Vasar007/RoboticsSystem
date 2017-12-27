#include "ModClient.h"


namespace clientTests
{

ModClient::ModClient(const int serverPortSending, const int serverReceiving, 
					 const std::string_view serverIP)
	: Client(serverPortSending, serverReceiving, serverIP),
	  mHasFinished(),
	  mStorage()
{
}

void ModClient::receiveDataNTimes(const int numberOfTimes)
{
	std::lock_guard<std::mutex> lockGuard{ mMutex };

	for (int step = 0; step < numberOfTimes; ++step)
	{
		receiveData(_receivingSocket);

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
					break;
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
								   const int numberOfIterations)
{
	const bool checkFirst	= checkCoordinates(firstPoint);
	const bool checkSecond	= checkCoordinates(secondPoint);

	for (int i = 0; i < numberOfIterations; ++i)
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
								   const int numberOfSteps)
{
	const vasily::RobotData directionalVector = (secondPoint - firstPoint) / numberOfSteps;
	vasily::RobotData robotData				  = firstPoint;

	bool isRight = false;

	if (checkCoordinates(firstPoint))
	{
		mStorage.push_back(firstPoint.toString());
		isRight = true;
	}

	for (int i = 0; i < numberOfSteps; ++i)
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

	if (isRight &&  checkCoordinates(secondPoint) && robotData != secondPoint)
	{
		mStorage.push_back(secondPoint.toString());
	}

	partialMovement(firstPoint, secondPoint, numberOfSteps);
}

std::thread ModClient::spawn(const int numberOfTimes)
{
	return std::thread(&ModClient::receiveDataNTimes, this, numberOfTimes);
}

} // namespace clientTests
