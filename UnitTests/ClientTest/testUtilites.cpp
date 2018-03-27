#include "testUtilites.h"


void specialAreEqual(const vasily::RobotData& expected, const vasily::RobotData& actual, 
    const std::wstring& comment = L"")
{
    for (std::size_t i = 0; i < vasily::RobotData::NUMBER_OF_COORDINATES; ++i)
    {
        const std::wstring message = L"not equal cordinates at position: "
                                     + std::to_wstring(i) + L" " + comment;
        Assert::AreEqual(expected.coordinates.at(i), actual.coordinates.at(i),
                         message.c_str());
    }

    for (std::size_t i = 0; i < vasily::RobotData::NUMBER_OF_PARAMETERS; ++i)
    {
        const std::wstring message = L"not equal parametrs at position: "
                                     + std::to_wstring(i) + L" " + comment;
        Assert::AreEqual(expected.parameters.at(i), actual.parameters.at(i),
                         message.c_str());
    }
}
