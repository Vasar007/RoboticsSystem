#ifndef SERVER_LAYER
#define SERVER_LAYER

#include <mutex>
#include <optional>

#include "Utilities.h"
#include "WinsockInterface.h"
#include "DelayManager.h"


namespace vasily
{

/**
 * \brief Class used to bind clients and robot as server.
 *        Also provides additional toolset to process received data.
 */
class ServerLayer : public WinsockInterface
{
public:
    /**
     * \brief Array of modes for layer how to process data.
     */
    enum class WorkMode
    {
        SAFE,
        UNSAFE
    };

    /**
     * \brief Array of constant to get parameters from config.
     */
    enum class Param : std::size_t
    {
        DEFAULT_IN_FILE_NAME,
        DEFAULT_OUT_FILE_NAME,
        DEFAULT_SERVER_IP,
        DEFAULT_SENDING_PORT_TO_SERVER,
        DEFAULT_RECEIVING_PORT_FROM_SERVER,
        DEFAULT_CLIENT_PORT,
        NUMBER_OF_MAIN_COORDINATES,
        MIN_COORDINATES,
        MAX_COORDINATES,
        RECONNECTION_DELAY
    };

    /**
     * \brief   Variable used to keep all default parameters and constants.
     * \details Using std::string instead of std::string_view because Logger constructor needs only
     *          std::string because of std::istream and std::ostream.
     */
    static const config::Config<std::string, std::string, std::string_view, int, int, int,
                                std::size_t, std::array<int, 3>, std::array<int, 3>, long long>
        CONFIG;

    /**
     * \brief                        Constructor that initializes sockets, connects to server and
     *                               creates socket to work with clients.
     * \param[in] serverSendingPort  Server port to send.
     * \param[in] serverRecivingPort Server port to recieve.
     * \param[in] serverIP           Server IP address for connection.
     * \param[in] layerPort          Additional port to communicate with clients.
     * \param[in] backlog            The maximum length of the queue of pending connections.
     * \param[in] workMode           Set work mode for layer to work with robot in safety
     *                               or unsafety.
     */
    explicit ServerLayer(
        const int serverSendingPort     = CONFIG.get<CAST(Param::DEFAULT_SENDING_PORT_TO_SERVER)>(),
        const int serverRecivingPort    = CONFIG.get<CAST(Param::DEFAULT_RECEIVING_PORT_FROM_SERVER)>(),
        const std::string_view serverIP = CONFIG.get<CAST(Param::DEFAULT_SERVER_IP)>(),
        const int layerPort             = CONFIG.get<CAST(Param::DEFAULT_CLIENT_PORT)>(),
        const int backlog               = 10,
        const WorkMode workMode         = WorkMode::SAFE);

    /**
     * \brief			Deleted copy constructor.
     * \param[in] other Other client object.
     */
                    ServerLayer(const ServerLayer& other)   = delete;

    /**
     * \brief			Deleted copy assignment operator.
     * \param[in] other Other client object.
     * \return			Returns nothing because it's deleted.
     */
    ServerLayer&    operator=(const ServerLayer& other)     = delete;

    /**
     * \brief			Deleted move constructor.
     * \param[in] other Other client object.
     */
                    ServerLayer(ServerLayer&& other)        = delete;

    /**
     * \brief			Deleted move assignment operator.
     * \param[in] other Other client object.
     * \return			Returns nothing because it's deleted.
     */
    ServerLayer&    operator=(ServerLayer&& other)          = delete;

    /**
     * \brief Destructor that closes client socket.
     */
    virtual         ~ServerLayer();

    /**
     * \brief Main method which starts infinite working loop.
     */
    void            run() override;

    /**
     * \brief Process sockets.
     */
    void            launch() override;

    /**
     * \brief  Get server IP address.
     * \return String which contains current server IP address.
     */
    std::string     getServerIP() const noexcept;

    /**
     * \brief                 Set server IP address.
     * \param[in] newServerIP New server IP address as string.
     */
    void		    setServerIP(const std::string_view newServerIP);

    /**
     * \brief Additional function that receives data from server.
     */
    void		    receiveFromServer();

    /**
     * \brief Additional function that receives data from clients.
     */
    void            receiveFromClients();

    /**
     * \brief				Check if given point is not out of working coordinates.
     * \param[in] robotData	Point to check.
     * \return				True if point is correct, false otherwise.
     */
    bool		    checkCoordinates(const RobotData& robotData) const;


protected:
    /**
     * \brief Receive buffer that is used to keep answers from clients.
     */
    char		_bufferForClient[_MAXRECV];

    /**
     * \brief Buffer that is used to keep clients addresses.
     */
    char		_messageWithIPForClient[_MAXRECV];

    /**
     * \brief Flag used to define active connection to client.
     */
    std::atomic_bool _isRunningForClient;

    /**
     * \brief Variable used to keep server IP address.
     */
    std::string _serverIP;

    /**
     * \brief Variable used to keep server port.
     */
    int			_layerPort;

    /**
     * \brief The maximum length of the queue of pending connections.
     */
    int			_backlog;

    /**
    * \brief Connected client socket used to send and receive data.
    */
    SOCKET		_clientSocket;

    /**
     * \brief Connected client socket used to send and receive data.
     */
    SOCKET		_layerSocket;

    /**
     * \brief Structure used to keep socket address to send and receive data.
     */
    SOCKADDR_IN	_layerSocketAddress;

    /**
     * \brief Variable used to keep server port to send.
     */
    int			_serverSendingPort;

    /**
     * \brief Variable used to keep server port to recieve.
     */
    int			_serverReceivingPort;

    /**
     * \brief Variable used to keep coordinate type from client.
     */
    std::optional<CoordinateSystem>	_coorninateSystem;

    /**
     * \brief Variable used to determine which layer are working in. 
     */
    WorkMode                _workMode;

    /**
     * \brief Last received data from client.
     */
    RobotData	            _lastReceivedPoint;

    /**
     * \brief Queue used to keeps received messages from clients.
     */
    std::deque<RobotData>   _messagesStorage;

    /**
     * \brief Mutex to lock thread for safety.
     */
    std::mutex              _mutex;

    /**
     * \brief Logger used to write received data to file.
     */
    logger::Logger          _logger;  // ORDER DEPENDENCY => 1.

    /**
     * \brief Class used to calculate delays.
     */
    DelayManager            _delayManager;  // ORDER DEPENDENCY => 2.


    /**
     * \brief Try to establish a connection to a specified socket again.
     */
    void tryReconnectToServer();

    /**
     * \brief          Check connection to robot every time.
     * \param[in] time Period time to check.
     */
    void checkConnectionToServer(const long long& time);

    /**
     * \brief Additional loop which has a handler for connections.
     */
    void process();

    /**
     * \brief Wait for clients connections.
     */
    void waitingForConnections();

    /**
     * \brief Main infinite working loop. All network logic should be placed here.
     */
    void waitLoop() override;

    /**
     * \brief Additional method which contains all connection calls. 
     */
    void doConnection();
};

} // namespace vasily

#endif // SERVER_LAYER
