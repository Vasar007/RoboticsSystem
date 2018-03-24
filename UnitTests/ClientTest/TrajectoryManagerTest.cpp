#include "TrajectoryManagerTest.h"

#include "TrajectoryManager.h"

/**
* \brief Namespace scope to test project.
*/
namespace clientTests
{

void TrajectoryManagerTest::positionalTrajectoryBuilding()
{
    const danila::TrajectoryManager trajectoryManager;
    std::vector<vasily::RobotData> result;
    result.push_back({ { 1, 1, 1, 1, 1, 1 },{ 10, 2, 0 } });
    result.push_back({ { 2, 2, 2, 2, 2, 2 },{ 10, 2, 0 } });
    result.push_back({ { 3, 3, 3, 3, 3, 3 },{ 10, 2, 0 } });
    result.push_back({ { 4, 4, 4, 4, 4, 4 },{ 10, 2, 0 } });

    vasily::ParsedResult parsedResult;
    parsedResult.isCorrect = true;
    parsedResult.points = result;

    const std::vector<vasily::RobotData> trajectory = 
        trajectoryManager.PositionalMovement(parsedResult);

    Assert::IsTrue(trajectory.size() == result.size(), L"Not equal numer of positions");
    for(size_t i=0;i < result.size(); ++i)
    {
        Assert::IsTrue(result[i].isEqual(trajectory[i]), L"Not equal positions");
    }
}

void TrajectoryManagerTest::circlicTrajectoryBuilding()
{
    const danila::TrajectoryManager trajectoryManager;
    std::vector<vasily::RobotData> result;
    result.push_back({ { 1, 1, 1, 1, 1, 1 },{ 10, 2, 0 } });
    result.push_back({ { 2, 2, 2, 2, 2, 2 },{ 10, 2, 0 } });
    result.push_back({ { 3, 3, 3, 3, 3, 3 },{ 10, 2, 0 } });
    result.push_back({ { 4, 4, 4, 4, 4, 4 },{ 10, 2, 0 } });

    const vasily::ParsedResult parsedResult{0,3,true,result};
    
    result.reserve(12);
    for(int i=0;i<2;++i)
    {
        for (int j = 0;j < 4;++j)
        {
            result.emplace_back(result[j]);
        }
    }

    const std::vector<vasily::RobotData> trajectory =
        trajectoryManager.CirclicMovement(parsedResult);

    Assert::IsTrue(trajectory.size() == result.size(), L"Not equal numer of positions");
    for (size_t i = 0;i < result.size(); ++i)
    {
        Assert::IsTrue(result[i].isEqual(trajectory[i]), L"Not equal positions");
    }
}

void TrajectoryManagerTest::partialTrajectoryBuildingWithOneBreakingPoint()
{
    const danila::TrajectoryManager trajectoryManager;
    std::vector<vasily::RobotData> inputPoints;
    inputPoints.push_back({ { 0, 0, 0, 0, 0, 0 },{ 10, 2, 0 } });
    inputPoints.push_back({ { 100, 0, 0, 0, 0, 0 },{ 10, 2, 0 } });
    inputPoints.push_back({ { 100, 100, 0, 0, 0, 0 },{ 10, 2, 0 } });
    
    const vasily::ParsedResult parsedResult{ 0,5,true,inputPoints };

    std::vector<vasily::RobotData> result;
    result.push_back({ { 0, 0, 0, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 40, 0, 0, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 80, 0, 0, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 100, 20, 0, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 100, 60, 0, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 100, 100, 0, 0, 0, 0 },{ 10, 2, 0 } });


    const std::vector<vasily::RobotData> trajectory =
        trajectoryManager.PartialMovement(parsedResult);


    Assert::IsTrue(trajectory.size() == result.size(), L"Not equal numer of positions");
    for (size_t i = 0;i < result.size(); ++i)
    {
        Assert::IsTrue(result[i].isEqual(trajectory[i]), L"Not equal positions");
    }
}

void TrajectoryManagerTest::partialTrajectoryBuildingWithManyBreakingPoints()
{
    const danila::TrajectoryManager trajectoryManager;
    std::vector<vasily::RobotData> inputPoints;
    inputPoints.push_back({ { 0, 0, 0, 0, 0, 0 },{ 10, 2, 0 } });
    inputPoints.push_back({ { 100, 0, 0, 0, 0, 0 },{ 10, 2, 0 } });
    inputPoints.push_back({ { 100, 100, 0, 0, 0, 0 },{ 10, 2, 0 } });
    inputPoints.push_back({ { 100, 100, 100, 0, 0, 0 },{ 10, 2, 0 } });
    inputPoints.push_back({ { 100, 100, 0, 0, 0, 0 },{ 10, 2, 0 } });

    const vasily::ParsedResult parsedResult{ 0,8,true,inputPoints };

    std::vector<vasily::RobotData> result;
    result.push_back({ { 0, 0, 0, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 50, 0, 0, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 100, 0, 0, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 100, 50, 0, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 100, 100, 0, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 100, 100, 50, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 100, 100, 100, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 100, 100, 50, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 100, 100, 0, 0, 0, 0 },{ 10, 2, 0 } });


    const std::vector<vasily::RobotData> trajectory =
        trajectoryManager.PartialMovement(parsedResult);


    Assert::IsTrue(trajectory.size() == result.size(), L"Not equal numer of positions");
    for (size_t i = 0;i < result.size(); ++i)
    {
        Assert::IsTrue(result[i].isEqual(trajectory[i]), L"Not equal positions");
    }
}

void TrajectoryManagerTest::partialTrajectoryBuildingInOneStep()
{
    const danila::TrajectoryManager trajectoryManager;
    std::vector<vasily::RobotData> inputPoints;
    inputPoints.push_back({ { 0, 0, 0, 0, 0, 0 },{ 10, 2, 0 } });
    inputPoints.push_back({ { 100, 0, 0, 0, 0, 0 },{ 10, 2, 0 } });
    inputPoints.push_back({ { 100, 100, 0, 0, 0, 0 },{ 10, 2, 0 } });
    inputPoints.push_back({ { 100, 100, 100, 0, 0, 0 },{ 10, 2, 0 } });
    inputPoints.push_back({ { 100, 100, 0, 0, 0, 0 },{ 10, 2, 0 } });

    const vasily::ParsedResult parsedResult{ 0,1,true,inputPoints };

    std::vector<vasily::RobotData> result;
    result.push_back({ { 0, 0, 0, 0, 0, 0 },{ 10, 2, 0 } });
    result.push_back({ { 100, 100, 0, 0, 0, 0 },{ 10, 2, 0 } });


    const std::vector<vasily::RobotData> trajectory =
        trajectoryManager.PartialMovement(parsedResult);


    Assert::IsTrue(trajectory.size() == result.size(), L"Not equal numer of positions");
    for (size_t i = 0;i < result.size(); ++i)
    {
        Assert::IsTrue(result[i].isEqual(trajectory[i]), L"Not equal positions");
    }
}

}