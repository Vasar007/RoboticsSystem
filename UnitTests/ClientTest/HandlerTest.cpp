
#include "HandlerTest.h"

#include "testUtilites.h"
#include "Handler.h"

/**
 * \brief Namespace scope to test project.
 */
namespace clientTests
{

void HandlerTest::incorrectSymbols()
{
    constexpr std::string_view command = { "p|3w|1 2 3 4 5 ;6 10 2 0|10 20r 30 40 5|0 60 10 2 0",
                                            52 };
    vasily::Handler handler;
    vasily::RobotData robotData = { { 1, 2, 3, 4, 5, 6 },{ 10, 2, 0 } };

    handler.appendCommand(command, robotData);
    const vasily::ParsedResult parsedResult = handler.getParsedResult();

    Assert::IsFalse(parsedResult.isCorrect, L"Mistake not found");
}

void HandlerTest::incorrectNumberOfIterations()
{
    constexpr std::string_view command = { "p|0|1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0",48 };
    vasily::Handler handler;
    vasily::RobotData robotData = { { 1, 2, 3, 4, 5, 6 },{ 10, 2, 0 } };

    handler.appendCommand(command, robotData);
    const vasily::ParsedResult parsedResult = handler.getParsedResult();

    Assert::IsFalse(parsedResult.isCorrect, L"Mistake not found");
}
    
void HandlerTest::incorrectNumberOfCordinates()
{
    constexpr std::string_view command = { "c|10|1 2 3 4 6 10 2 0|10 20 30 40 50 60 70 10 2 0",50 };
    vasily::Handler handler;
    vasily::RobotData robotData = { { 1, 2, 3, 4, 5, 6 },{ 10, 2, 0 } };

    handler.appendCommand(command, robotData);
    const vasily::ParsedResult parsedResult = handler.getParsedResult();

    Assert::IsFalse(parsedResult.isCorrect, L"Mistake not found");
}

void HandlerTest::handlerTwoPointsParsing()
{
    constexpr std::string_view command = { "p|3|1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0",48 };
    vasily::Handler handler;
    std::vector<vasily::RobotData> result
    {
        { {  1,  2,  3,  4,  5,  6 }, { 10, 2, 0 } },
        { { 10, 20, 30, 40, 50, 60 }, { 10, 2, 0 } }
    };

    handler.appendCommand(command, *result.begin());
    vasily::ParsedResult parsedResult = handler.getParsedResult();

    Assert::IsTrue(parsedResult.isCorrect, L"There is no mistake");
    Assert::AreEqual(CAST(vasily::Handler::State::PARTIAL), CAST(handler.getCurrentState()),
        L"not right state parsed");
    Assert::AreEqual(result.size(), parsedResult.points.size(), L"Not right number of points");
    Assert::AreEqual(3, parsedResult.numberOfIterations, L"Not right number of iterations");
    for (std::size_t i = 0; i < result.size(); ++i)
    {
        const std::wstring message = L"in " + std::to_wstring(i) + L" point!";
        specialAreEqual(result[i], parsedResult.points[i], message);
    }
}

void HandlerTest::handlerManyPointsParsing()
{
    constexpr std::string_view command =
    { "c|3|1 1 1 1 1 1 10 2 0|2 2 2 2 2 2 10 2 0|3 3 3 3 3 3 10 2 0|4 4 4 4 4 4 10 2 0",80 };
    vasily::Handler handler;
    std::vector<vasily::RobotData> result
    {
        { { 1, 1, 1, 1, 1, 1 }, { 10, 2, 0 } },
        { { 2, 2, 2, 2, 2, 2 }, { 10, 2, 0 } },
        { { 3, 3, 3, 3, 3, 3 }, { 10, 2, 0 } },
        { { 4, 4, 4, 4, 4, 4 }, { 10, 2, 0 } }
    };

    handler.appendCommand(command, *result.begin());
    vasily::ParsedResult parsedResult = handler.getParsedResult();

    Assert::IsTrue(parsedResult.isCorrect, L"There is no mistake");
    Assert::AreEqual(CAST(vasily::Handler::State::CIRCLIC), CAST(handler.getCurrentState()),
        L"not right state parsed");
    Assert::AreEqual(result.size(), parsedResult.points.size(), L"Not right number of points");
    Assert::AreEqual(3, parsedResult.numberOfIterations, L"Not right number of iterations");
    for (std::size_t i = 0; i < result.size(); ++i)
    {
        const std::wstring message = L"in " + std::to_wstring(i) + L" point!";
        specialAreEqual(result[i], parsedResult.points[i], message);
    }
}

void HandlerTest::handlerManyPointsParsingWiyhoutIterations()
{
    constexpr std::string_view command =
    { "o|1 1 1 1 1 1 10 2 0|2 2 2 2 2 2 10 2 0|3 3 3 3 3 3 10 2 0|4 4 4 4 4 4 10 2 0",78 };
    vasily::Handler handler;
    std::vector<vasily::RobotData> result
    {
        { { 1, 1, 1, 1, 1, 1 }, { 10, 2, 0 } },
        { { 2, 2, 2, 2, 2, 2 }, { 10, 2, 0 } },
        { { 3, 3, 3, 3, 3, 3 }, { 10, 2, 0 } },
        { { 4, 4, 4, 4, 4, 4 }, { 10, 2, 0 } }
    };

    handler.appendCommand(command, *result.begin());
    vasily::ParsedResult parsedResult = handler.getParsedResult();

    Assert::IsTrue(parsedResult.isCorrect, L"There is no mistake");
    Assert::AreEqual(CAST(vasily::Handler::State::POSITIONAL), CAST(handler.getCurrentState()),
        L"not right state parsed");
    Assert::AreEqual(result.size(), parsedResult.points.size(), L"Not right number of points");
    for (std::size_t i = 0; i < result.size(); ++i)
    {
        const std::wstring message = L"in " + std::to_wstring(i) + L" point!";
        specialAreEqual(result[i], parsedResult.points[i], message);
    }
}

} // namespace clientTests
