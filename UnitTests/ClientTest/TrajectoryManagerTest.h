#ifndef TRAJECTORY_MANAGER_TEST_H
#define TRAJECTORY_MANAGER_TEST_H

#include <CppUnitTest.h>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace clientTests
{

TEST_CLASS(TrajectoryManagerTest)
{
public:
    
    /**
     * \brief Test of building positional trajectory.
     */
    TEST_METHOD(positionalTrajectoryBuilding);

    /**
     * \brief Test of building circlic trajectory.
     */
    TEST_METHOD(circlicTrajectoryBuilding);

    /**
     * \brief Test of building partial trajectory with one breaking point.
     */
    TEST_METHOD(partialTrajectoryBuildingWithOneBreakingPoint);

    /**
     * \brief Test of building partial trajectory with many breaking points.
     */
    TEST_METHOD(partialTrajectoryBuildingWithManyBreakingPoints);

    /**
     * \brief Test of building partial trajectory with one moving.
     */
    TEST_METHOD(partialTrajectoryBuildingInOneStep);
};

} // namespace clientTests

#endif // TRAJECTORY_MANAGER_TEST_H
