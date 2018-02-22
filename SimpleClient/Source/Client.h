#ifndef CLIENT_H
#define CLIENT_H

#include <chrono>

#include "WinsockInterface.h"
#include "Utilities.h"
#include "Handler.h"
#include "TenzoMath.h"


namespace vasily
{

/**
 * \brief Typical realization of client used for interaction with robot server.
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

protected:
	/**
	 * \brief Array of states to work in circlic mode.
	 */
	enum class CirclicState
	{
		SEND_FIRST,
		WAIT_FIRST_ANSWER,
		SEND_SECOND,
		WAIT_SECOND_ANSWER
	};


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
	int			_serverSendingPort{};

	/**
	 * \brief Variable used to keep server port to recieve.
	 */
	int			_serverReceivingPort{};

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
	RobotData			_waitAnswer;
							
	/**
	 * \brief Flag used to define if client needs to wait answer from robot.
	 */
	std::atomic_bool	_isNeedToWait;
							
	/**
	 * \brief Current state of work in circle.
	 */
	CirclicState		_circlicState;

	/**
	 * \brief Work mode for client, initialize when object created.
	 */
	WorkMode            _workMode;

	/**
	 * \brief Logger used to write received data to file.
	 */
	logger::Logger      _logger;

	/**
	 * \brief Class-wrapper used to work with strain gauge. 
	 */
	nikita::TenzoMath   _tenzoMath;

	/**
	 * \brief Default file name for input.
	 */
	static constexpr char       _DEFAULT_IN_FILE_NAME[]             = "in.txt";

	/**
	 * \brief Default file name for output.
	 */
	static constexpr char       _DEFAULT_OUT_FILE_NAME[]            = "out.txt";

	/**
	 * \brief Default value for server IP.
	 */
	static constexpr char		_DEFAULT_SERVER_IP[]	            = "192.168.1.21";

	/**
	 * \brief Default value for sending port.
	 */
	static constexpr int		_DEFAULT_SENDING_PORT_TO_SERVER	    = 59002;
													
	/**                                             
	 * \brief Default value for receiving port.     
	 */                                             
	static constexpr int		_DEFAULT_RECEIVING_PORT_FROM_SERVER	= 59003;

	/**
	 * \brief Constant number of coordinates to check to avoid "magic number".
	 */
	static constexpr std::size_t _MAIN_COORDINATES		            = 3u;

	/**
	 * \brief Default (beginning) robot position.
	 */
	static constexpr RobotData	_DEFAULT_POSITION{ RobotData::DEFAULT_CORDINATES,
												   RobotData::DEFAULT_PARAMETERS };

	/**
	 * \brief Array contains minimum value for first 3 coordinates (x, y, z).
	 */
	static constexpr std::array<int, _MAIN_COORDINATES> _MIN_COORDS{ 830'000, -400'000, 539'000 };

	/**
	 * \brief Array contains maximum value for first 3 coordinates (x, y, z).
	 */
	static constexpr std::array<int, _MAIN_COORDINATES> _MAX_COORDS{ 1'320'000, 400'000, 960'000 };


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
	void		checkConnection(const std::atomic_int64_t& time);

	/**
	 * \brief							Work with robot in circlic mode.
	 * \details							Now function works only with 2 points!
	 * \param[in] firstPoint			First point to send and in which robot should return.
	 * \param[in] secondPoint			Second point for circlic movement.
	 * \param[in] numberOfIterations	Number of iterations in circlic movement.
	 * \code
	 * Enter command: c|1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0|5
	 * \endcode
	 */
	void		circlicMovement(const RobotData& firstPoint, const RobotData& secondPoint, 
								const int numberOfIterations = 1);

	/**
	 * \brief					Work with robot in partial mode.
	 * \details					Now function works only with 2 points!
	 * \param[in] firstPoint	Start point.
	 * \param[in] secondPoint	End point.
	 * \param[in] numberOfSteps	Number of steps for which robot should move from start to end point.
	 * \code
	 * Enter command: p|1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0|3
	 * \endcode
	*/
	void		partialMovement(const RobotData& firstPoint, const RobotData& secondPoint,
								const int numberOfSteps = 1);


public:
	/**
	 * \brief Used to define break point for transmitter.
	 */
	std::atomic_bool    isNeedToUpdate;

	/**
	 * \brief Keep last sent robot's point.
	 */
	RobotData           lastSentPoint;


	/**
	 * \brief					Constructor that initializes sockets and connects to server.
	 * \param[in] serverPort	Server port for connection.
	 * \param[in] serverIP		Server IP address for connection.
	 * \param[in] workMode      Set work mode for client to work with robot straightforward or
	 *                          indirect.
	 */
	explicit	Client(const int serverPort, const std::string_view serverIP,
					   const WorkMode workMode = WorkMode::INDIRECT);

	/**
	 * \brief						    Constructor that initializes sockets and connects to server.
	 * \param[in] serverSendingPort	    Server port to send.
	 * \param[in] serverReceivingPort	Server port to recieve.
	 * \param[in] serverIP			    Server IP address for connection.
	 * \param[in] workMode              Set work mode for client to work with robot straightforward
	 *                                  or indirect.
	 */
	explicit	Client(const int serverSendingPort      = _DEFAULT_SENDING_PORT_TO_SERVER, 
					   const int serverReceivingPort    = _DEFAULT_RECEIVING_PORT_FROM_SERVER,
					   const std::string_view serverIP  = _DEFAULT_SERVER_IP,
					   const WorkMode workMode          = WorkMode::INDIRECT);

	/**	
	 * \brief Default destructor.
	 */
	virtual     ~Client() noexcept				= default;

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
	 * \brief				Move constructor.
	 * \param[out] other	Other client object.
	 */
				Client(Client&& other) noexcept;

	/**
	 * \brief				Move assignment operator.
	 * \param[out] other	Other client object.
	 * \return				Returns an object with all moved data.
	 */
	Client&		operator=(Client&& other) noexcept;


	/**
	 * \brief	Get server IP address.
	 * \return	String which contains current server IP address.
	 */
	std::string getServerIP() const;

	/**
	 * \brief					Set server IP address.
	 * \param[in] newServerIP	New server IP address as string.
	 */
	void		setServerIP(const std::string_view newServerIP);

	/**
	 * \brief	Get current duration.
	 * \return	Measured time between start point and some event.
	 */
	std::chrono::duration<double> getDuration() const;

	/**
	 * \brief   Get actual point to interact.
	 * \return  RobotData structure.
	 */
	RobotData   getRobotData() const;

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
	 * \brief				Check if given point is not out of working coordinates.
	 * \param[in] robotData	Point to check.
	 * \return				True if point is correct, false otherwise.
	 */
	bool		checkCoordinates(const RobotData& robotData) const;


	/**
	 * \brief				Check coordinates and if it's right sends to robot.
	 * \param[in] robotData Point to send.
	 * \return				True if coordinates sent, false otherwise.
	 */
	bool		sendCoordinates(const RobotData& robotData);

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


	// Friendly swapping fuction.
	template <class T>
	friend void utils::swap(T& first, T& second) noexcept;
};

} // namespace vasily

#endif // CLIENT_H
