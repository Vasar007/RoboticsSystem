#include "ModClient.h"


namespace clientTests
{

ModClient::ModClient(const int serverPortSending, const int serverReceiving, 
                     const std::string_view serverIP, const Client::WorkMode workMode)
    : Client(serverPortSending, serverReceiving, serverIP, workMode),
      hasFinished(false)
{
}

void ModClient::receiveDataNTimes(const int numberOfTimes)
{
    std::lock_guard<std::mutex> lockGuard{ mutex };

    for (int step = 0; step < numberOfTimes; ++step)
    {
        receiveData(_receivingSocket, _messageWithIP, _buffer);

        if (_isNeedToWait.load())
        {
            _isNeedToWait.store(false);
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

    hasFinished.store(true);
}

void ModClient::sendCoordinatesMod(const vasily::RobotData& robotData)
{
    storage.push_back(robotData.toString());
    sendCoordinates(robotData);
}

void ModClient::circlicMovementMod(const vasily::RobotData& firstPoint,
                                   const vasily::RobotData& secondPoint, 
                                   const int numberOfIterations)
{
    for (int i = 0; i < numberOfIterations; ++i)
    {
        storage.push_back(firstPoint.toString());
        storage.push_back(secondPoint.toString());
    }
    storage.push_back(firstPoint.toString());

    circlicMovement(firstPoint, secondPoint, numberOfIterations);
}

void ModClient::partialMovementMod(const vasily::RobotData& firstPoint,
                                   const vasily::RobotData& secondPoint, 
                                   const int numberOfSteps)
{
    const vasily::RobotData directionalVector = (secondPoint - firstPoint) / numberOfSteps;
    vasily::RobotData robotData				  = firstPoint;

    storage.push_back(firstPoint.toString());

    for (int i = 0; i < numberOfSteps; ++i)
    {
        robotData += directionalVector;
        storage.push_back(robotData.toString());
    }

    if (robotData != secondPoint)
    {
        storage.push_back(secondPoint.toString());
    }

    partialMovement(firstPoint, secondPoint, numberOfSteps);
}

std::thread ModClient::spawn(const int numberOfTimes)
{
    return std::thread(&ModClient::receiveDataNTimes, this, numberOfTimes);
}

} // namespace clientTests
