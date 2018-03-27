#ifndef SERVER_H
#define SERVER_H

#include <chrono>
#include <optional>

#include "Utilities.h"
#include "WinsockInterface.h"


namespace vasily
{
    
/**
 * \brief Pretty simple imitator FanucM20iA.
 */
class ServerImitator : public WinsockInterface
{
public:
    /**
     * \brief Array of constant to get parameters from config.
     */
    enum class Param : std::size_t
    {
        DEFAULT_IN_FILE_NAME,
        DEFAULT_OUT_FILE_NAME
    };

    /**
     * \brief   Variable used to keep all default parameters and constants.
     * \details Using std::string instead of std::string_view because Logger constructor needs only
     *          std::string because of std::istream and std::ostream.
     */
    static const config::Config<std::string, std::string> CONFIG;

    /**
     * \brief					     Constructor which initializes sockets and bindes ports to
     *                               them.
     * \param[in] clientSendingPort	 Port for connection.
     * \param[in] clientRecivingPort Port for connection.
     * \param[in] backlog		     Number of connections allowed on the incoming queue.
     */
    explicit		ServerImitator(const int clientSendingPort, const int clientRecivingPort,
                                   const int backlog = 10);

    /**
     * \brief Default destructor.
     */
    virtual         ~ServerImitator() noexcept					= default;

    /**
     * \brief			Deleted copy constructor.
     * \param[in] other Other client object.
     */
                    ServerImitator(const ServerImitator& other) = delete;

    /**
     * \brief			Deleted copy assignment operator.
     * \param[in] other Other client object.
     * \return			Returns nothing because it's deleted.
     */
    ServerImitator&	operator=(const ServerImitator& other)		= delete;

    /**
     * \brief			 Move constructor.
     * \param[out] other Other client object.
     */
                    ServerImitator(ServerImitator&& other) noexcept;

    /**
     * \brief			 Move assignment operator.
     * \param[out] other Other client object.
     * \return			 Returns an object with all moved data.
     */
    ServerImitator&	operator=(ServerImitator&& other) noexcept;

    /**
     * \brief Main method which starts infinite working loop.
     */
    void			run() override;

    /**
     * \brief Fuction processes sockets (call 'bind' and 'listen').
     */
    void			launch() override;


protected:
    /**
     * \brief Variable used to keep sending port.
     */
    int					    _clientSendingPort;
                    
    /**
     * \brief Variable used to keep reciving port.
     */
    int					    _clientReceivingPort;
                    
    /**
     * \brief The maximum length of the queue of pending connections.
     */
    int					    _backlog;

    /**
     * \brief Connected client socket used to send data.
     */
    SOCKET				    _clientSendingSocket;

    /**
     * \brief Connected client socket used to receive data.
     */
    SOCKET				    _clientReceivingSocket;

    /**
     * \brief Variable used to keep coordinate type from client.
     */
    std::optional<CoordinateSystem>	_coorninateSystem;

    /**
     * \brief Logger used to write received data to file.
     */
    logger::Logger          _logger;

    /**
     * \brief Last received data from client.
     */
    RobotData               _lastReceivedData;


    /**
     * \brief Main infinite working loop. Network logic to interacte with clients.
     */
    void			waitLoop() override;

    /**
     * \brief Additional loop which has a handler for connections.
     */
    void			process();

    /**
     * \brief Wait for clients connections.
     */
    void			waitingForConnections();

    /**
     * \brief               Calculate duration for currrent movement section.
     * \details             Used to calculate duration for sleeping before sending answer to client.
     * \param[in] robotData New point of movement.
     * \return              Approximately duration in milliseconds. 
     */
    std::chrono::milliseconds calculateDuration(const RobotData& robotData);
};

} // namespace vasily

#endif // SERVER_H
