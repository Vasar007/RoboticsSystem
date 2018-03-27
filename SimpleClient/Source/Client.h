#ifndef CLIENT_H
#define CLIENT_H

#include <chrono>

#include "Handler.h"
#include "Utilities.h"
#include "WinsockInterface.h"   // INCLUDE ORDER DEPENDENCY -> 1 (because of "Windows.h")
#include "TenzoMath.h"          // INCLUDE ORDER DEPENDENCY -> 2
#include "TrajectoryManager.h"


namespace vasily
{

/**
 * \brief Typical implementation of client used for interaction with robot server.
 */
class Client : public WinsockInterface
{
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
    enum class Param : std::size_t
    {
        DEFAULT_IN_FILE_NAME,
        DEFAULT_OUT_FILE_NAME,
        DEFAULT_SERVER_IP,
        DEFAULT_SENDING_PORT_TO_SERVER,
        DEFAULT_RECEIVING_PORT_FROM_SERVER,
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
     * \brief Used to define break point for transmitter.
     */
    std::atomic_bool    isNeedToUpdate;

    /**
     * \brief Keep last sent robot's point.
     */
    RobotData           lastSentPoint;


    /**
     * \brief				 Constructor that initializes sockets and connects to server.
     * \param[in] serverPort Server port for connection.
     * \param[in] serverIP	 Server IP address for connection.
     * \param[in] workMode   Set work mode for client to work with robot straightforward or
     *                       indirect.
     */
    explicit	Client(const int serverPort, const std::string_view serverIP,
                       const WorkMode workMode = WorkMode::INDIRECT);

    /**
     * \brief						  Constructor that initializes sockets and connects to server.
     * \param[in] serverSendingPort	  Server port to send.
     * \param[in] serverReceivingPort Server port to recieve.
     * \param[in] serverIP			  Server IP address for connection.
     * \param[in] workMode            Set work mode for client to work with robot straightforward
     *                                or indirect.
     */
    explicit	Client(
        const int serverSendingPort     = CONFIG.get<CAST(Param::DEFAULT_SENDING_PORT_TO_SERVER)>(), 
        const int serverReceivingPort   = CONFIG.get<CAST(Param::DEFAULT_RECEIVING_PORT_FROM_SERVER)>(),
        const std::string_view serverIP = CONFIG.get<CAST(Param::DEFAULT_SERVER_IP)>(),
        const WorkMode workMode         = WorkMode::INDIRECT);

    /**	
     * \brief Default destructor.
     */
    virtual     ~Client()       				= default;

    /**
     * \brief			Deleted copy constructor.
     * \param[in] other Other client object.
     */
                Client(const Client& other)		= delete;

    /**
     * \brief			Deleted copy assignment operator.
     * \param[in] other Other client object.
     * \return			Returns nothing because it's deleted.
     */
    Client&		operator=(const Client& other)	= delete;

    /**
     * \brief			 Move constructor.
     * \param[out] other Other client object.
     */
                Client(Client&& other) noexcept;

    /**
     * \brief			 Move assignment operator.
     * \param[out] other Other client object.
     * \return			 Returns an object with all moved data.
     */
    Client&		operator=(Client&& other) noexcept;


    /**
     * \brief  Get server IP address.
     * \return String which contains current server IP address.
     */
    std::string getServerIP() const noexcept;

    /**
     * \brief				  Set server IP address.
     * \param[in] newServerIP New server IP address as string.
     */
    void		setServerIP(const std::string_view newServerIP);

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
    void		run() override;

    /**
     * \brief Fuction processes sockets (call 'connect').
     */
    void		launch() override;

    /**
     * \brief Additional fuction that receives data from server.
     */
    void		receive();

    /**
     * \brief				send coordinate to robot.
     * \param[in] robotData Point to send.
     */
    void		sendCoordinates(const RobotData& robotData);

    /**
    * \brief	    	send coordinates to robot.
    * \param[in] points List of points for sending.
    */
    void		sendCoordinates(const std::vector<RobotData>& points);

    /**
     * \brief						Send coordinate system to robot.
     * \param[in] coordinateSystem	Coordinate system to send.
     */
    void		sendCoordinateSystem(const CoordinateSystem coordinateSystem) const;

    /**
     * \brief Calibrate strain gauge.
     */
    void        tenzoCalibration();

    /**
     * \brief Do work with strain gauge.
     */
    void        workWithTenzo();


protected:
    /**
     * \brief Structure which contains data that is used for interaction with robot.
     */
    RobotData	_robotData;

    /**
     * \brief Variable used to keep server IP address.
     */
    std::string _serverIP;

    /**
     * \brief Variable used to keep server port.
     */
    int			_serverPort;

    /**
     * \brief Variable used to keep server port to send.
     */
    int			_serverSendingPort;

    /**
     * \brief Variable used to keep server port to recieve.
     */
    int			_serverReceivingPort;

    /**
     * \brief User data handler.
     */
    Handler		_handler;
    
    /**
     * \brief Starting position to measure the time.
     */
    std::chrono::time_point<std::chrono::steady_clock>	_start;

    /**
     * \brief Measured time between start point and some event.
     */
    std::chrono::duration<double>						_duration{};

    /**
     * \brief Data used to send and with we compare answer from robot if it needs.
     */
    RobotData		        	_waitAnswer;

    /**
     * \brief Work mode for client, initialize when object created.
     */
    WorkMode                    _workMode;

    /**
     * \brief Logger used to write received data to file.
     */
    logger::Logger              _logger;

    /**
     * \brief Class-wrapper used to work with strain gauge. 
     */
    nikita::TenzoMath           _tenzoMath;

    /**
     * \brief Class used ot generate trajectoryes.
     */
    danila::TrajectoryManager   _trajectoryManager;


    /**
     * \brief Try to establish a connection to a specified socket again.
     */
    void		tryReconnect();

    /**
     * \brief Main infinite working loop. Network logic to interacte with server.
     */
    void		waitLoop() override;

    /**
     * \brief			Check connection to robot every time.
     * \param[in] time	Period time to check.
     */
    void		checkConnection(const long long time);
};

} // namespace vasily

#endif // CLIENT_H
