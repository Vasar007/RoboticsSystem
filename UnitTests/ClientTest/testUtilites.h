#ifndef TEST_UTILITES_H
#define TEST_UTILITES_H

#include <CppUnitTest.h>

#include <RobotData/RobotData.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void myAreEqual(const vasily::RobotData& expected, const vasily::RobotData& actual,
    const std::wstring& comment);

#endif