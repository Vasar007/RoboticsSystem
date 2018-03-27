#ifndef HANDLER_TEST_H
#define HANDLER_TEST_H

#include <CppUnitTest.h>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


/**
 * \brief Namespace scope to test project.
 */
namespace clientTests
{
    
/**
 * \brief Set of tests for client.
 */
TEST_CLASS(HandlerTest)
{
public:
    /**
     * \brief Test for checking parsing of command with incorrect symbols.
     */
    TEST_METHOD(incorrectSymbols);

    /**
     * \brief Test for checking parsing of command with incorrect number of iterations.
     */
    TEST_METHOD(incorrectNumberOfIterations);

    /**
     * \brief Test for checking parsing of command with incorrect number of coordinates in point.
     */
    TEST_METHOD(incorrectNumberOfCordinates);

    /**
     * \brief Test for checking parsing of command with two points.
     */
    TEST_METHOD(handlerTwoPointsParsing);


    /**
     * \brief Test for checking parsing of command with many points.
     */
    TEST_METHOD(handlerManyPointsParsing);

    /**
     * \brief Test for checking parsing of command with many points,
     *        and without number of iterations.
     */
    TEST_METHOD(handlerManyPointsParsingWiyhoutIterations);

};

} // namespace clientTests

#endif // HANDLER_TEST_H