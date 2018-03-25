#include "TrajectoryManagerTest.h"

#include "testUtilites.h"
#include "TrajectoryManager.h"

/**
* \brief Namespace scope to test project.
*/
namespace clientTests
{

void TrajectoryManagerTest::positionalTrajectoryBuilding()
{
    const danila::TrajectoryManager trajectoryManager;
    const std::vector<vasily::RobotData> result
    {
        { { 1, 1, 1, 1, 1, 1 }, { 10, 2, 0 } },
        { { 2, 2, 2, 2, 2, 2 }, { 10, 2, 0 } },
        { { 3, 3, 3, 3, 3, 3 }, { 10, 2, 0 } },
        { { 4, 4, 4, 4, 4, 4 }, { 10, 2, 0 } }
    };

    const vasily::ParsedResult parsedResult{ 0, 3, true, result };

    const std::vector<vasily::RobotData> trajectory = 
        trajectoryManager.PositionalMovement(parsedResult);

    Assert::AreEqual(result.size(), trajectory.size(), L"Not equal numer of positions");
    for (std::size_t i = 0; i < result.size(); ++i)
    {
        const std::wstring message = L"in " + std::to_wstring(i) + L" point!";
        myAreEqual(result[i], trajectory[i], message);
    }
}

void TrajectoryManagerTest::circlicTrajectoryBuilding()
{
    const danila::TrajectoryManager trajectoryManager;
    const std::vector<vasily::RobotData> result
    {
        { { 1, 1, 1, 1, 1, 1 }, { 10, 2, 0 } },
        { { 2, 2, 2, 2, 2, 2 }, { 10, 2, 0 } },
        { { 3, 3, 3, 3, 3, 3 }, { 10, 2, 0 } },
        { { 4, 4, 4, 4, 4, 4 }, { 10, 2, 0 } },
        { { 1, 1, 1, 1, 1, 1 }, { 10, 2, 0 } },
        { { 2, 2, 2, 2, 2, 2 }, { 10, 2, 0 } },
        { { 3, 3, 3, 3, 3, 3 }, { 10, 2, 0 } },
        { { 4, 4, 4, 4, 4, 4 }, { 10, 2, 0 } },
        { { 1, 1, 1, 1, 1, 1 }, { 10, 2, 0 } },
        { { 2, 2, 2, 2, 2, 2 }, { 10, 2, 0 } },
        { { 3, 3, 3, 3, 3, 3 }, { 10, 2, 0 } },
        { { 4, 4, 4, 4, 4, 4 }, { 10, 2, 0 } }
    };

    const vasily::ParsedResult parsedResult{0,3,true,
    {
        { { 1, 1, 1, 1, 1, 1 }, { 10, 2, 0 } },
        { { 2, 2, 2, 2, 2, 2 }, { 10, 2, 0 } },
        { { 3, 3, 3, 3, 3, 3 }, { 10, 2, 0 } },
        { { 4, 4, 4, 4, 4, 4 }, { 10, 2, 0 } }
    } 
    };

    const std::vector<vasily::RobotData> trajectory =
        trajectoryManager.CirclicMovement(parsedResult);

    Assert::AreEqual(result.size(), trajectory.size(), L"Not equal numer of positions");
    for (std::size_t i = 0; i < result.size(); ++i)
    {
        const std::wstring message = L"in " + std::to_wstring(i) + L" point!";
        myAreEqual(result[i], trajectory[i], message);
    }
}

void TrajectoryManagerTest::partialTrajectoryBuildingWithOneBreakingPoint()
{
    const danila::TrajectoryManager trajectoryManager;
    
    const vasily::ParsedResult parsedResult{ 0, 5, true,
    {
        { {   0,   0, 0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100,   0, 0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100, 0, 0, 0, 0 }, { 10, 2, 0 } }
    } 
    };

    std::vector<vasily::RobotData> result
    {
        { {   0,   0, 0, 0, 0, 0 }, { 10, 2, 0 } },
        { {  40,   0, 0, 0, 0, 0 }, { 10, 2, 0 } },
        { {  80,   0, 0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100,  20, 0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100,  60, 0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100, 0, 0, 0, 0 }, { 10, 2, 0 } }
    };

    const std::vector<vasily::RobotData> trajectory =
        trajectoryManager.PartialMovement(parsedResult);


    Assert::AreEqual(result.size(), trajectory.size(), L"Not equal numer of positions");
    for (std::size_t i = 0; i < result.size(); ++i)
    {
        const std::wstring message = L"in " + std::to_wstring(i) + L" point!";
        myAreEqual(result[i], trajectory[i], message);
    }
}

void TrajectoryManagerTest::partialTrajectoryBuildingWithManyBreakingPoints()
{
    const danila::TrajectoryManager trajectoryManager;

    const vasily::ParsedResult parsedResult{ 0,8,true,
    {
        { {   0,   0,   0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100,   0,   0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100,   0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100, 100, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100,   0, 0, 0, 0 }, { 10, 2, 0 } }
    }
    };

    std::vector<vasily::RobotData> result
    {
        { {   0,   0,   0, 0, 0, 0 }, { 10, 2, 0 } },
        { {  50,   0,   0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100,   0,   0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100,  50,   0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100,   0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100,  50, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100, 100, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100,  50, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100,   0, 0, 0, 0 }, { 10, 2, 0 } }
    };


    const std::vector<vasily::RobotData> trajectory =
        trajectoryManager.PartialMovement(parsedResult);


    Assert::AreEqual(result.size(), trajectory.size(), L"Not equal numer of positions");
    for (std::size_t i = 0; i < result.size(); ++i)
    {
        const std::wstring message = L"in " + std::to_wstring(i) + L" point!";
        myAreEqual(result[i], trajectory[i], message);
    }
}

void TrajectoryManagerTest::partialTrajectoryBuildingInOneStep()
{
    const danila::TrajectoryManager trajectoryManager;

    const vasily::ParsedResult parsedResult{ 0, 1, true, 
    {
        { {   0,   0,   0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100,   0,   0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100,   0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100, 100, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100,   0, 0, 0, 0 }, { 10, 2, 0 } }
    }
    };

    std::vector<vasily::RobotData> result
    { 
        { {   0,   0, 0, 0, 0, 0 }, { 10, 2, 0 } },
        { { 100, 100, 0, 0, 0, 0 }, { 10, 2, 0 } } 
    };


    const std::vector<vasily::RobotData> trajectory =
        trajectoryManager.PartialMovement(parsedResult);


    Assert::AreEqual(result.size(), trajectory.size(), L"Not equal numer of positions");
    for (std::size_t i = 0; i < result.size(); ++i)
    {
        const std::wstring message = L"in " + std::to_wstring(i) + L" point!";
        myAreEqual(result[i], trajectory[i], message);
    }
}

} // namespace clientTests