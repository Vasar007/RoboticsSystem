#include "HandlerTest.h"

#include "Handler.h"

/**
* \brief Namespace scope to test project.
*/
namespace clientTests
{

void HandlerTest::IncorrectSymbols()
{
    constexpr std::string_view command = { "p|3w|1 2 3 4 5 ;6 10 2 0|10 20r 30 40 5|0 60 10 2 0",
                                            52 };
    vasily::Handler handler;
    vasily::RobotData robotData = { { 1, 2, 3, 4, 5, 6 },{ 10, 2, 0 } };

    handler.appendCommand(command, robotData);
    const vasily::ParsedResult parsedResult = handler.getParsedResult();

    Assert::IsTrue(!parsedResult.isCorrect, L"Mistake not found");
}

void HandlerTest::IncorrectNumberOfIterations()
{
    constexpr std::string_view command = { "p|0|1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0",48 };
    vasily::Handler handler;
    vasily::RobotData robotData = { { 1, 2, 3, 4, 5, 6 },{ 10, 2, 0 } };

    handler.appendCommand(command, robotData);
    const vasily::ParsedResult parsedResult = handler.getParsedResult();

    Assert::IsTrue(!parsedResult.isCorrect, L"Mistake not found");
}
    
void HandlerTest::IncorrectNumberOfCordinates()
{
    constexpr std::string_view command = { "c|10|1 2 3 4 6 10 2 0|10 20 30 40 50 60 70 10 2 0",50 };
    vasily::Handler handler;
    vasily::RobotData robotData = { { 1, 2, 3, 4, 5, 6 },{ 10, 2, 0 } };

    handler.appendCommand(command, robotData);
    const vasily::ParsedResult parsedResult = handler.getParsedResult();

    Assert::IsTrue(!parsedResult.isCorrect, L"Mistake not found");
}

void HandlerTest::HandlerTwoPointsParsing()
{
    constexpr std::string_view command = { "p|3|1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0",48 };
    vasily::Handler handler;
    std::vector<vasily::RobotData> result;
    result.push_back({ { 1, 2, 3, 4, 5, 6 },{ 10, 2, 0 } });
    result.push_back({ { 10, 20, 30, 40, 50, 60 },{ 10, 2, 0 } });

    handler.appendCommand(command, *result.begin());
    vasily::ParsedResult parsedResult = handler.getParsedResult();

    Assert::IsTrue(handler.getCurrentState() == vasily::Handler::State::PARTIAL,
        L"not right state parsed");
    Assert::IsTrue(parsedResult.points.size() == result.size(), L"Not right number of points");
    Assert::IsTrue(parsedResult.numberOfIterations == 3, L"Not right number of iterations");
    Assert::IsTrue(parsedResult.points[0].isEqual(result[0]), L"Not right first point");
    Assert::IsTrue(parsedResult.points[1].isEqual(result[1]), L"Not right second point");
}

void HandlerTest::HandlerManyPointsParsing()
{
    constexpr std::string_view command =
    { "c|3|1 1 1 1 1 1 10 2 0|2 2 2 2 2 2 10 2 0|3 3 3 3 3 3 10 2 0|4 4 4 4 4 4 10 2 0",80 };
    vasily::Handler handler;
    std::vector<vasily::RobotData> result;
    result.push_back({ { 1, 1, 1, 1, 1, 1 },{ 10, 2, 0 } });
    result.push_back({ { 2, 2, 2, 2, 2, 2 },{ 10, 2, 0 } });
    result.push_back({ { 3, 3, 3, 3, 3, 3 },{ 10, 2, 0 } });
    result.push_back({ { 4, 4, 4, 4, 4, 4 },{ 10, 2, 0 } });

    handler.appendCommand(command, *result.begin());
    vasily::ParsedResult parsedResult = handler.getParsedResult();

    Assert::IsTrue(handler.getCurrentState() == vasily::Handler::State::CIRCLIC,
        L"not right state parsed");
    Assert::IsTrue(parsedResult.points.size() == result.size(), L"Not right number of points");
    Assert::IsTrue(parsedResult.numberOfIterations == 3, L"Not right number of iterations");
    for (size_t i = 0; i < result.size();++i)
    {
        Assert::IsTrue(parsedResult.points[i].isEqual(result[i]), L"Not right point");
    }
}

void HandlerTest::HandlerManyPointsParsingWiyhoutIterations()
{
    constexpr std::string_view command =
    { "o|1 1 1 1 1 1 10 2 0|2 2 2 2 2 2 10 2 0|3 3 3 3 3 3 10 2 0|4 4 4 4 4 4 10 2 0",78 };
    vasily::Handler handler;
    std::vector<vasily::RobotData> result;
    result.push_back({ { 1, 1, 1, 1, 1, 1 },{ 10, 2, 0 } });
    result.push_back({ { 2, 2, 2, 2, 2, 2 },{ 10, 2, 0 } });
    result.push_back({ { 3, 3, 3, 3, 3, 3 },{ 10, 2, 0 } });
    result.push_back({ { 4, 4, 4, 4, 4, 4 },{ 10, 2, 0 } });

    handler.appendCommand(command, *result.begin());
    vasily::ParsedResult parsedResult = handler.getParsedResult();

    Assert::IsTrue(handler.getCurrentState() == vasily::Handler::State::POSITIONAL,
        L"not right state parsed");
    Assert::IsTrue(parsedResult.points.size() == result.size(), L"Not right number of points");
    for (size_t i = 0; i < result.size();++i)
    {
        Assert::IsTrue(parsedResult.points[i].isEqual(result[i]), L"Not right point");
    }
}

}