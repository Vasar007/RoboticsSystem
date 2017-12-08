#ifndef CLIENT_H
#define CLIENT_H

#include <thread>
#include <chrono>

#include "WinsockInterface.h"
#include "RobotData.h"
#include "Utilities.h"
#include "Handler.h"


namespace vasily
{

/**
 * \brief Typical realization of client used for interaction with robot server.
 */
class Client : public WinsockInterface
{
public:
	/**
	 * \brief Array of coordinates type.
	 */
	enum class CoordinateType
	{
		JOINT = 1,
		WORLD = 2
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
	int			_serverPortSending;

	/**
	 * \brief Variable used to keep server port to recieve.
	 */
	int			_serverPortReceiving;

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
	std::chrono::duration<double>						_duration;

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
	 * \brief Default value for server IP.
	 */
	static constexpr char		_DEFAULT_SERVER_IP[]	= "192.168.0.21";

	/**
	 * \brief Default value for sending port.
	 */
	static constexpr int		_DEFAULT_SENDING_PORT	= 59002;
													
	/**                                             
	 * \brief Default value for receiving port.     
	 */                                             
	static constexpr int		_DEFAULT_RECEIVING_PORT	= 59003;

	/**
	 * \brief Default (beginning) robot position.
	 */
	static constexpr RobotData	_DEFAULT_POSITION{ 985'000, 0, 940'000, -180'000, 0, 0, 10, 2, 0 };

	/**
	 * \brief Constant number of coordinates to check to avoid "magic number".
	 */
	static constexpr std::size_t _MAIN_COORDINATES							= 3u;

	/**
	 * \brief Array contains minimum value for first 3 coordinates (x, y, z).
	 */
	static constexpr std::array<int, _MAIN_COORDINATES> _MIN_COORDS{ 830'000, -400'000, 539'000 };

	/**
	 * \brief Array contains maximum value for first 3 coordinates (x, y, z).
	 */
	static constexpr std::array<int, _MAIN_COORDINATES> _MAX_COORDS{ 1'320'000, 317'000, 960'000 };


	/**
	 * \brief Function tries to establishe a connection to a specified socket again.
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
	 * \brief							Function works with robot in circlic mode.
	 * \details							Now function works only with 2 points!
	 * \param[in] firstPoint			First point to send and in which robot should return.
	 * \param[in] secondPoint			Second point for circlic movement.
	 * \param[in] numberOfIterations	Number of iterations in circlic movement.
	 */
	void		circlicProcessing(const RobotData& firstPoint, const RobotData& secondPoint, 
								  const std::size_t numberOfIterations = 1u);

	/**
	 * \brief					Function works with robot in partial mode.
	 * \details					Now function works only with 2 points!
	 * \param[in] firstPoint	Start point.
	 * \param[in] secondPoint	End point.
	 * \param[in] numberOfSteps Number of steps for which robot should move from start to end point.
	 */
	void		partialProcessing(const RobotData& firstPoint, const RobotData& secondPoint,
								  const std::size_t numberOfSteps = 1u);


public:
	/**
	 * \brief					Constructor that initializes sockets and connects to server.
	 * \param[in] serverPort	Server port for connection.
	 * \param[in] serverIP		Server IP address for connection.
	 */
	explicit	Client(const int serverPort, const std::string_view serverIP);

	/**
	 * \brief						Constructor that initializes sockets and connects to server.
	 * \param[in] serverPortSending	Server port to send.
	 * \param[in] serverReceiving	Server port to recieve.
	 * \param[in] serverIP			Server IP address for connection.
	 */
	explicit	Client(const int serverPortSending = _DEFAULT_SENDING_PORT, 
					   const int serverReceiving = _DEFAULT_RECEIVING_PORT,
					   const std::string_view serverIP = _DEFAULT_SERVER_IP);

	/**	
	 * \brief Default destructor.
	 */
				~Client()						= default;

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
	 * \brief	Function returns server IP address.
	 * \return	String which contains current server IP address.
	 */
	std::string getServerIP() const;

	/**
	 * \brief					Function sets server IP address.
	 * \param[in] newServerIP	New server IP address as string.
	 */
	void		setServerIP(const std::string_view newServerIP);

	/**
	 * \brief	Function returns current duration.
	 * \return	Measured time between start point and some event.
	 */
	std::chrono::duration<double> getDuration() const;

	/**
	 * \brief Main method which starts infinite working loop.
	 */
	void		run() override;

	/**
	 * \brief Fuction processes sockets (call 'connect').
	 */
	void		launch() override;

	/**
	 * \brief							Function launches thread for circlic processing and
	 *									forwards parameters.
	 * \details							Now function works only with 2 points!
	 * \param[in] firstPoint			First point to send and in which robot should return.
	 * \param[in] secondPoint			Second point for circlic movement.
	 * \param[in] numberOfIterations	Number of iterations in circlic movement.
	 * \code
	 * Enter command: c 1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0|5
	 * \endcode
	 */
	void		circlicMovement(const RobotData& firstPoint, const RobotData& secondPoint, 
								const std::size_t numberOfIterations);

	/**
	 * \brief					Function launches thread for partial processing and 
	 *							forwards parameters.
	 * \details					Now function works only with 2 points!
	 * \param[in] firstPoint	Start point.
	 * \param[in] secondPoint	End point.
	 * \param[in] numberOfSteps	Number of steps for which robot should move from start to end point.
	 * \code
	 * Enter command: p 1 2 3 4 5 6 10 2 0|10 20 30 40 50 60 10 2 0|3
	 * \endcode
	*/
	void		partialMovement(const RobotData& firstPoint, const RobotData& secondPoint,
								const std::size_t numberOfSteps);

	/**
	 * \brief Additional fuction that receives data from server.
	 */
	void		receive();

	/**
	 * \brief				Function checks if given point is not out of working coordinates.
	 * \param[in] robotData	Point to check.
	 * \return				True if point is correct, false otherwise.
	 */
	bool		checkCoordinates(const RobotData& robotData) const;


	/**
	 * \brief				Function checks coordinates and if it's right sends to robot.
	 * \param[in] robotData Point to send.
	 * \return				True if coordinates sent, false otherwise.
	 */
	bool		sendCoordinates(const RobotData& robotData);

	/**
	 * \brief						Function sends coordinate system to robot.
	 * \param[in] coordinateType	Coordinate system to send.
	 */
	void		sendCoordinateType(const CoordinateType coordinateType) const;


	// Friendly swapping fuction.
	template <class T>
	friend void utils::swap(T& first, T& second) noexcept;
};

}

#endif // CLIENT_H
