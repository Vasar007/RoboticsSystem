#ifndef TEST_SERVER_H
#define TEST_SERVER_H

#include <mutex>

#include "ServerImitator.h"


namespace clientTests
{

/**
 * \brief Simple test server for client.
 */
struct TestServer final : public vasily::ServerImitator
{
    /**
     * \brief Array of constant to get parameters from config.
     */
    enum class Param : std::size_t
    {
        PORT_SENDING,
        PORT_RECEIVING
    };

    /**
     * \brief Variable used to keep all default parameters and constants.
     */
    static constexpr config::Config<int, int> TEST_CONFIG
    {
        9998,
        9999
    };

    /**
     * \brief Mutex to lock thread for safety.
     */
    std::mutex					mutex;

    /**
     * \brief Additional flag used to define successful connection.
     */
    std::atomic_bool			hasConnected;

    /**
     * \brief Additional flag used to define end of receiving method.
     */
    std::atomic_bool			hasFinished;

    /**
     * \brief Array of received data from client to check.
     */
    std::vector<std::string>	storage;


    /**
     * \brief					Cinstructor for test server.
     * \param[in] sendingPort	Port for connection.
     * \param[in] recivingPort	Port for connection.
     * \param[in] backlog		Number of connections allowed on the incoming queue.
     */
    explicit    TestServer(const int sendingPort  = TEST_CONFIG.get<CAST(Param::PORT_SENDING)>(),
                           const int recivingPort = TEST_CONFIG.get<CAST(Param::PORT_RECEIVING)>(),
                           const int backlog      = 10);

    /**
     * \brief					Receiv data from receiving socket.
     * \param[in] numberOfTimes Number of times to allow connections.
     */
    void		receiveDataNTimes(const int numberOfTimes);
};

} // namespace clientTests

#endif // TEST_SERVER_H
