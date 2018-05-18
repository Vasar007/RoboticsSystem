#ifndef CLIENT_H
#define CLIENT_H

#include <chrono>

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "Handler.h"
#include "Utilities.h"
#include "TrajectoryManager.h"


namespace vasily
{

/**
 * \brief Typical implementation of client used for interaction with robot server.
 */
class Client : public QObject
{
    Q_OBJECT
public:
    /**
     * \brief Array of modes for client how to work with robot.
     */
    enum class WorkMode
    {
        STRAIGHTFORWARD,
        INDIRECT
    };

    /**
     * \brief Array of constant to get parameters from config.
     */
    enum Param : std::size_t
    {
        DEFAULT_IN_FILE_NAME,
        DEFAULT_OUT_FILE_NAME,
        DEFAULT_SERVER_IP,
        DEFAULT_RECEIVING_PORT_FROM_SERVER,
        DEFAULT_SENDING_PORT_TO_SERVER,
        RECONNECTION_DELAY
    };

    /**
     * \brief   Variable used to keep all default parameters and constants.
     * \details Using std::string instead of std::string_view because Logger constructor needs only
     *          std::string because of std::istream and std::ostream.
     */
    static const config::Config<std::string, std::string, std::string_view, int, int, long long>
        CONFIG;


    /**
     * \brief				Constructor that initializes sockets and connects to server.
     * \param[in] layerPort Server port for connection.
     * \param[in] serverIP	Server IP address for connection.
     * \param[in] workMode  Set work mode for client to work with robot straightforward or
     *                      indirect.
     * \param[in] parent    The necessary data for Qt.
     */
    explicit    Client(const int layerPort, const std::string_view serverIP,
                       const WorkMode workMode = WorkMode::INDIRECT, QObject* parent = nullptr);

    /**
     * \brief						  Constructor that initializes sockets and connects to server.
     * \param[in] serverReceivingPort Server port to recieve.
     * \param[in] serverSendingPort	  Server port to send.
     * \param[in] serverIP            Server IP address for connection.
     * \param[in] workMode            Set work mode for client to work with robot straightforward
     *                                or indirect.
     * \param[in] parent              The necessary data for Qt.
     */
    explicit    Client(
        const int serverReceivingPort   = CONFIG.get<Param::DEFAULT_RECEIVING_PORT_FROM_SERVER>(),
        const int serverSendingPort     = CONFIG.get<Param::DEFAULT_SENDING_PORT_TO_SERVER>(), 
        const std::string_view serverIP = CONFIG.get<Param::DEFAULT_SERVER_IP>(),
        const WorkMode workMode         = WorkMode::STRAIGHTFORWARD,
        QObject* parent                 = nullptr);

    /**	
     * \brief Default destructor.
     */
    virtual     ~Client() = default;

    /**
     * \brief           Deleted copy constructor.
     * \param[in] other Other client object.
     */
                Client(const Client& other) = delete;

    /**
     * \brief           Deleted copy assignment operator.
     * \param[in] other Other client object.
     * \return          Returns nothing because it's deleted.
     */
    Client&     operator=(const Client& other) = delete;

    /**
     * \brief            Deleted move constructor.
     * \param[out] other Other client object.
     */
                Client(Client&& other) = delete;

    /**
     * \brief            Deleted move assignment operator.
     * \param[out] other Other client object.
     * \return           Returns nothing because it's deleted.
     */
    Client&	    operator=(Client&& other) = delete;

    /**
     * \brief  Get current duration.
     * \return Measured time between start point and some event.
     */
    std::chrono::duration<double> getDuration() const noexcept;

    /**
     * \brief  Get actual point to interact.
     * \return RobotData structure.
     */
    RobotData   getRobotData() const noexcept;

    /**
     * \brief Main method which starts infinite working loop.
     */
    void        run();

    /**
     * \brief Fuction processes sockets (call 'connect').
     */
    void        launch();

    /**
     * \brief				send coordinate to robot.
     * \param[in] robotData Point to send.
     */
    void        sendCoordinates(const RobotData& robotData);

    /**
    * \brief	    	send coordinates to robot.
    * \param[in] points List of points for sending.
    */
    void        sendCoordinates(const std::vector<RobotData>& points);

    /**
     * \brief						Send coordinate system to robot.
     * \param[in] coordinateSystem	Coordinate system to send.
     */
    void        sendCoordinateSystem(const CoordinateSystem coordinateSystem) const;


signals:
    /**
     * \brief          Notify client to send data to layer or server depend on work mode.
     * \param[in] data Data to be send.
     */
    void signalToSend(const QByteArray& data) const;


private slots:
    /**
     * \brief Receive data from layer.
     */
    void slotReadFromLayer();

    /**
     * \brief Receive data from server.
     */
    void slotReadFromServer();

    /**
     * \brief Process server disconnection from layer.
     */
    void slotServerDisconnected();

    /**
     * \brief          Send data to layer after notifying from signal.
     * \param[in] data Data to be send.
     */
    void slotSendDataToLayer(const QByteArray& data) const;

    /**
     * \brief          Send data to server after notifying from signal.
     * \param[in] data Data to be send.
     */
    void slotSendDataToServer(const QByteArray& data) const;


protected:
    /**
     * \brief Implementation of type-safe output printer.
     */
    printer::Printer&                                  _printer = printer::Printer::getInstance();

    /**
     * \brief Variable used to keep layer port.
     */
    int                                                _layerPort;

    /**
     * \brief Connected to layer socket used to send and receive data.
     */
    std::unique_ptr<QTcpSocket>                        _socketForLayer;

    /**
     * \brief Variable used to keep recieving server port.
     */
    int	                                               _serverReceivingPort;

    /**
     * \brief Connected to server socket used to receive data.
     */
    std::unique_ptr<QTcpSocket>                        _receivingSocket;

    /**
     * \brief Variable used to keep sending server port.
     */
    int                                                _serverSendingPort;

    /**
     * \brief Connected to server socket used to send data.
     */
    std::unique_ptr<QTcpSocket>                        _sendingSocket;

    /**
     * \brief Structure which contains data that is used for interaction with robot.
     */
    RobotData                                          _robotData;

    /**
     * \brief Container that keeps distance between every two last points.
     */
    std::vector<double>                                _distance;

    /**
     * \brief Container that keeps velocity of robot movement.
     */
    std::vector<double>                                _velocity;

    /**
     * \brief Container that keeps duration of robot movement.
     */
    std::vector<double>                                _time;

    /**
     * \brief Flag used to determine whether a response is accepted from the server.
     */
    std::atomic_bool                                   _isReceive{};

    /**
     * \brief Keep last reached robot's point.
     */
    vasily::RobotData                                  _lastReachedPoint;

    /**
     * \brief Variable used to keep server IP address.
     */
    std::string                                        _serverIP;

    /**
     * \brief User data handler.
     */
    Handler                                            _handler;
    
    /**
     * \brief Starting position to measure the time.
     */
    std::chrono::time_point<std::chrono::steady_clock> _start;

    /**
     * \brief Measured time between start point and some event.
     */
    std::chrono::duration<double>                      _duration{};

    /**
     * \brief Work mode for client, initialize when object created.
     */
    WorkMode                                           _workMode;

    /**
     * \brief Logger used to write received data to file.
     */
    logger::Logger                                     _logger;

    /**
     * \brief Class used ot generate trajectoryes.
     */
    danila::TrajectoryManager                          _trajectoryManager;


    /**
     * \brief                        Establishe a connection to a specified socket.
     * \param[in] port               Port for connection.
     * \param[in] ip                 IP address for connection.
     * \param[out] socketToConnect   A descriptor identifying an unconnected socket.
     * \param[in] isNeedToSendSystem Flag specifies whether to send the coordinate system.
     * \param[in] msecs              Time which Qt waits until the socket is connected. If this
     *                               parameter is -1, function will not time out.
     * \return                       If no error occurs, connect returns true, false otherwise.
     */
    bool        tryConnect(const int port, const std::string& ip, QTcpSocket* const socketToConnect,
                           const bool isNeedToSendSystem = false, const int msecs = 3000) const;

    /**
     * \brief Try to establish a connection to server again.
     */
    void        tryReconnect();

    /**
     * \brief          Send data on a connected socket.
     * \param[in] data A buffer containing the data to be transmitted.
     */
    void        sendData(const std::string& data) const;

    /**
     * \brief Main infinite working loop. Network logic to interacte with server are placed here.
     */
    void        waitLoop();

    /**
     * \brief          Check connection to robot every time.
     * \param[in] time Period time to check.       
     * \code
     * Example of usage:
     * constexpr long long kTime = 2000;
     * std::thread checkThread(&Client::checkConnection, this, std::cref(kTime));
     * checkThread.detach();
     * \endcode
     */
    void        checkConnection(const long long time);

    void updateVertices(const double time, const vasily::RobotData& robotData);
};

} // namespace vasily

#endif // CLIENT_H
