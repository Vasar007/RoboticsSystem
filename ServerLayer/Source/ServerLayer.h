#ifndef SERVER_LAYER
#define SERVER_LAYER

#include "Utilities.h"
#include "WinsockInterface.h"


namespace vasily
{

class ServerLayer : public WinsockInterface
{
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
     * \brief Structure which contains data that is used for interaction with robot.
     */
    RobotData	_robotData;

    /**
     * \brief Logger used to write received data to file.
     */
    logger::Logger          _logger;

    /**
     * \brief Default file name for input.
     */
    static constexpr char   _DEFAULT_IN_FILE_NAME[]             = "in.txt";

    /**
     * \brief Default file name for output.
     */
    static constexpr char   _DEFAULT_OUT_FILE_NAME[]            = "out.txt";

    /**
     * \brief Default value for server IP.
     */
    static constexpr char	_DEFAULT_SERVER_IP[]                = "192.168.0.21";

    /**
     * \brief Default value for sending port.
     */
    static constexpr int	_DEFAULT_SENDING_PORT_TO_SERVER     = 59002;

    /**
     * \brief Default value for receiving port.
     */
    static constexpr int	_DEFAULT_RECEIVING_PORT_FROM_SERVER = 59003;

    /**
     * \brief Constant number of server layer port to send and receive message for clients.
     */
    static constexpr int	_DEFAULT_CLIENT_PORT                = 8888;


    /**
     * \brief Try to establish a connection to a specified socket again.
     */
    void tryReconnectToServer();

    /**
     * \brief			Check connection to robot every time.
     * \param[in] time	Period time to check.
     */
    void checkConnectionToServer(const std::atomic_int64_t& time);

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

    
public:
    explicit ServerLayer(const int serverSendingPort        = _DEFAULT_SENDING_PORT_TO_SERVER,
                         const int serverRecivingPort       = _DEFAULT_RECEIVING_PORT_FROM_SERVER,
                         const std::string_view serverIP    = _DEFAULT_SERVER_IP,
                         const int layerPort                = _DEFAULT_CLIENT_PORT,
                         const int backlog = 10);

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
     * \brief Default destructor.
     */
    virtual         ~ServerLayer() noexcept;

    /**
     * \brief Main method which starts infinite working loop.
     */
    void            run() override;

    /**
     * \brief Process sockets.
     */
    void            launch() override;

    /**
     * \brief	Get server IP address.
     * \return	String which contains current server IP address.
     */
    std::string     getServerIP() const;

    /**
     * \brief					Set server IP address.
     * \param[in] newServerIP	New server IP address as string.
     */
    void		    setServerIP(const std::string_view newServerIP);

    /**
     * \brief Additional fuction that receives data from server.
     */
    void		    receiveFromServer();
};

} // namespace vasily

#endif // SERVER_LAYER
