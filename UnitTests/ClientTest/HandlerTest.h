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
    TEST_METHOD(IncorrectSymbols);

    /**
    * \brief Test for checking parsing of command with incorrect number of iterations.
    */
    TEST_METHOD(IncorrectNumberOfIterations);

    /**
    * \brief Test for checking parsing of command with incorrect number of coordinates in point.
    */
    TEST_METHOD(IncorrectNumberOfCordinates);

    /**
    * \brief Test for checking parsing of command with two points.
    */
    TEST_METHOD(HandlerTwoPointsParsing);


    /**
    * \brief Test for checking parsing of command with many points.
    */
    TEST_METHOD(HandlerManyPointsParsing);

    /**
    * \brief Test for checking parsing of command with many points,
    *        and without number of iterations.
    */
    TEST_METHOD(HandlerManyPointsParsingWiyhoutIterations);

};

}
#endif // HANDLER_TEST_H