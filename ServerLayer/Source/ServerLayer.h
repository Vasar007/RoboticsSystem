#ifndef SERVER_LAYER
#define SERVER_LAYER

#include <optional>

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

#include "Utilities.h"
#include "DelayManager.h"


namespace vasily
{

/**
 * \brief Class used to bind clients and robot as server.
 *        Also provides additional toolset to process received data.
 */
class ServerLayer : public QObject
{
    Q_OBJECT
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
        DEFAULT_RECEIVING_PORT_FROM_SERVER,
        DEFAULT_SENDING_PORT_TO_SERVER,
        DEFAULT_LAYER_PORT,
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
     * \brief                         Constructor that initializes sockets, connects to server and
     *                                creates socket to work with clients.
     * \param[in] serverReceivingPort Server port to receiive.
     * \param[in] serverSendingPort   Server port to send.
     * \param[in] serverIP            Server IP address for connection.
     * \param[in] layerPort           Additional port to communicate with clients.
     * \param[in] workMode            Set work mode for layer to work with robot in safety
     *                                or unsafety.
     * \param[in] parent              The necessary data for Qt.
     */
    explicit ServerLayer(
        const int serverReceivingPort   = CONFIG.get<CAST(Param::DEFAULT_RECEIVING_PORT_FROM_SERVER)>(),
        const int serverSendingPort     = CONFIG.get<CAST(Param::DEFAULT_SENDING_PORT_TO_SERVER)>(),
        const std::string_view serverIP = CONFIG.get<CAST(Param::DEFAULT_SERVER_IP)>(),
        const int layerPort             = CONFIG.get<CAST(Param::DEFAULT_LAYER_PORT)>(),
        const WorkMode workMode         = WorkMode::SAFE,
        QObject* parent                 = nullptr);

    /**
     * \brief Destructor that closes client socket.
     */
    virtual         ~ServerLayer() = default;

    /**
     * \brief           Deleted copy constructor.
     * \param[in] other Other client object.
     */
                    ServerLayer(const ServerLayer& other) = delete;

    /**
     * \brief           Deleted copy assignment operator.
     * \param[in] other Other client object.
     * \return          Returns nothing because it's deleted.
     */
    ServerLayer&    operator=(const ServerLayer& other) = delete;

    /**
     * \brief           Deleted move constructor.
     * \param[in] other Other client object.
     */
                    ServerLayer(ServerLayer&& other) = delete;

    /**
     * \brief           Deleted move assignment operator.
     * \param[in] other Other client object.
     * \return          Returns nothing because it's deleted.
     */
    ServerLayer&    operator=(ServerLayer&& other) = delete;

    /**
     * \brief Process sockets.
     */
    void            launch();

    /**
     * \brief               Check if given point is not out of working coordinates.
     * \param[in] robotData	Point to check.
     * \return              True if point is correct, false otherwise.
     */
    bool		    checkCoordinates(const RobotData& robotData) const;


signals:
    /**
     * \brief          Notify layer to send data to client.
     * \param[in] data Data to be send.
     */
    void signalToSendToClient(const QByteArray& data) const;

    /**
     * \brief          Notify layer to send data to server.
     * \param[in] data Data to be send.
     */
    void signalToSendToServer(const QByteArray& data) const;

    /**
     * \brief Notify layer that message from client was received and
     *        needs to be processed.
     */
    void signalProcessMessagesStorage();

    /**
     * \brief Notify layer that answer from server was received and
     *        needs to be processed.
     */
    void signalProcessAnswersStorage();


private slots:
    /**
     * \brief Process new client connection to layer.
     */
    void slotNewClientConnection();

    /**
     * \brief Process client disconnection from layer.
     */
    void slotClientDisconnected();

    /**
     * \brief Process server disconnection from layer.
     */
    void slotServerDisconnected();

    /**
     * \brief Receive data from client
     */
    void slotReadFromClient();

    /**
     * \brief Receive data from server.
     */
    void slotReadFromServer();

    /**
     * \brief          Send data to client after notifying from signal.
     * \param[in] data Data to be send.
     */
    void slotSendDataToClient(const QByteArray& data) const;

    /**
     * \brief          Send data to server after notifying from signal.
     * \param[in] data Data to be send.
     */
    void slotSendDataToServer(const QByteArray& data) const;

    /**
     * \brief Process received message from client after notifying from signal.
     */
    void slotProcessMessagesStorage();

    /**
     * \brief Process received answer from server after notifying from signal.
     */
    void slotProcessAnswersStorage();


protected:
    /**
     * \brief A set of constants used to specify the recipient in the send function.
     */
    enum class Whereto
    {
        SERVER,
        CLIENT
    };

    /**
     * \brief Implementation of type-safe output printer.
     */
    printer::Printer&               _printer = printer::Printer::getInstance();

    /**
     * \brief Variable used to keep reciving port.
     */
    int                             _layerPort;

    /**
     * \brief Connected socket used to receive data.
     */
    std::unique_ptr<QTcpServer>	    _layerSocket;

    /**
     * \brief Pointer to socket used to work with clients.
     */
    QTcpSocket*                     _clientSocket;

    /**
     * \brief Variable used to keep recieving server port.
     */
    int	                            _serverReceivingPort;

    /**
     * \brief Connected to server socket used to receive data.
     */
    std::unique_ptr<QTcpSocket>     _receivingSocket;

    /**
     * \brief Variable used to keep sending server port.
     */
    int                             _serverSendingPort;

    /**
     * \brief Connected to server socket used to send data.
     */
    std::unique_ptr<QTcpSocket>     _sendingSocket;

    /**
     * \brief Variable used to keep server IP address.
     */
    std::string                     _serverIP;

    /**
     * \brief Variable used to keep coordinate type from client.
     */
    std::optional<CoordinateSystem>	_coorninateSystem;

    /**
     * \brief Variable used to determine which layer are working in. 
     */
    WorkMode                        _workMode;

    /**
     * \brief Last received data from client.
     */
    RobotData                       _lastReceivedPoint;

    /**
     * \brief Queue used to keeps received messages from clients.
     */
    std::deque<RobotData>           _messagesStorage;

    /**
     * \brief Queue used to keeps received messages from server.
     */
    std::deque<std::string>         _answersStorage;

    /**
     * \brief Logger used to write received data to file.
     */
    logger::Logger                  _logger;  // ORDER DEPENDENCY => 1.

    /**
     * \brief Class used to calculate delays.
     */
    DelayManager                    _delayManager;  // ORDER DEPENDENCY => 2.


    /**
     * \brief          Check connection to robot every time.
     * \param[in] time Period time to check.
     */
    void checkConnectionToServer(const long long time);

    /**
     * \brief               Send data on a connected socket.
     * \param[in] data      A buffer containing the data to be transmitted.
     * \param[in] recipient Specifies the recipient.
     */
    void sendData(const std::string& data, const Whereto recipient) const;

    /**
     * \brief                        Establishe a connection to a specified socket.
     * \param[in] port               Port for connection.
     * \param[in] ip                 IP address for connection.
     * \param[out] socketToConnect   A descriptor identifying an unconnected socket.
     * \param[in] msecs              Time which Qt waits until the socket is connected. If this
     *                               parameter is -1, function will not time out.
     * \return                       If no error occurs, connect returns true, false otherwise.
     */
    bool tryConnect(const int port, const std::string& ip, QTcpSocket* socketToConnect,
                    const int msecs = 3000) const;

    /**
     * \brief Try to establish a connection to server again.
     */
    void tryReconnectToServer();

    /**
     * \brief Additional method which contains all connection calls. 
     */
    void doConnection();
};

} // namespace vasily

#endif // SERVER_LAYER
