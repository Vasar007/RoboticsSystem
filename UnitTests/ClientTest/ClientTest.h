#ifndef CLIENT_TEST_H
#define CLIENT_TEST_H

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
TEST_CLASS(ClientTest)
{
public:
    /**
     * \brief Simple send-receive test.
     */
    TEST_METHOD(sendReceiveAbility);
    
    /**
     * \brief Test on dangerous zone.
     */
    TEST_METHOD(dangerusZone);

};

} // namespace clientTests

#endif // CLIENT_TEST_H
