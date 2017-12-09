#include "RobotData.hpp"

constexpr int test()
{
    vasily::RobotData robotData{ 1, 2, 3, 4, 5, 6, 7, 8 ,9 };
    vasily::RobotData robotData2{};
    robotData2 = robotData2 - robotData;
    return robotData2.mParameters.at(0u);
}
