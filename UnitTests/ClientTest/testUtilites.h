#ifndef TEST_UTILITES_H
#define TEST_UTILITES_H

#include <CppUnitTest.h>

#include <RobotData/RobotData.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

/**
 * \brief Function for checking if two points are equal using microsoft tests.
 * \param[in] expected Right point.
 * \param[in] actual   Tested point.
 * \param[in] comment  Description of point.
 */
void myAreEqual(const vasily::RobotData& expected, const vasily::RobotData& actual,
    const std::wstring& comment);

#endif