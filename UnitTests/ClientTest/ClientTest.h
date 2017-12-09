#ifndef CLIENT_TEST_H
#define CLIENT_TEST_H

#include "CppUnitTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


/**
 * \brief Namespace scope to test project.
 */
namespace clientTests
{

/**
 * \brief Set of tests for client.
 */
TEST_CLASS(ClientTest)
{
public:
	/**
	 * \brief Simple send-receive test.
	 */
	TEST_METHOD(testMethod1);
	
	/**
	 * \brief Test on dangerous zone.
	 */
	TEST_METHOD(testMethod2);
	
	/**
	 * \brief Circlic movement test.
	 */
	TEST_METHOD(testMethod3);
	
	/**
	 * \brief Partial movement test.
	 */
	TEST_METHOD(testMethod4);
};

}

#endif // CLIENT_TEST_H
