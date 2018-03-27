#ifndef MOD_CLIENT_H
#define MOD_CLIENT_H

#include <mutex>

#include "Client.h"


namespace clientTests
{

struct ModClient final : public vasily::Client
{
    /**
     * \brief Array of constant to get parameters from config.
     */
    enum class Param : std::size_t
    {
        SERVER_PORT,
        SERVER_PORT_SENDING,
        SERVER_PORT_RECEIVING,
        SERVER_IP
    };

    /**
     * \brief Variable used to keep all default parameters and constants.
     */
    static constexpr config::Config<int, int, int, std::string_view> TEST_CONFIG
    {
        8888,
        9999,
        9998,
        { "192.168.0.101", 14 }
    };

    /**
     * \brief Mutex to lock thread for safety.
     */
    std::mutex					mutex;

    /**
     * \brief Additional flag used to define end of receiving method.
     */
    std::atomic_bool			hasFinished;

    /**
     * \brief Array of sent data to server to check.
     */
    std::vector<std::string>	storage{};


    /**
     * \brief						Constructor that initializes sockets and connects to server.
     * \param[in] serverPortSending	Server port to send.
     * \param[in] serverReceiving	Server port to recieve.
     * \param[in] serverIP			Server IP address for connection.
     * \param[in] workMode          Set work mode for client to work with robot straightforward or
     *                              indirect.
     */
    explicit	ModClient(
        const int serverPortSending     = TEST_CONFIG.get<CAST(Param::SERVER_PORT_SENDING)>(),
        const int serverReceiving       = TEST_CONFIG.get<CAST(Param::SERVER_PORT_RECEIVING)>(),
        const std::string_view serverIP = TEST_CONFIG.get<CAST(Param::SERVER_IP)>(),
        const Client::WorkMode workMode = Client::WorkMode::INDIRECT);

    /**
     * \brief					Receive data from receiving socket.
     * \details					Create additional thread to receive data from sercer.
     * \param[in] numberOfTimes Number of times to allow connections.
     */
    void		receiveDataNTimes(const int numberOfTimes);

    /**
     * \brief				Check coordinates and if it's right sends to robot.
     * \param[in] robotData Point to send.
     */
    void		sendCoordinatesMod(const vasily::RobotData& robotData);

    /**
     * \brief					Spawn new thread to receive data from client.
     * \param[in] numberOfTimes Number of times to allow connections.
     * \return					Created thread.
     */
    std::thread	spawn(const int numberOfTimes);
};

} // namespace clientTests

#endif // MOD_CLIENT_H
