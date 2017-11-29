#ifndef CLIENT_H
#define CLIENT_H

#include <thread>
#include <atomic>
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
private:
	enum class CirclicState
	{
		SEND_FIRST,
		WAIT_FIRST_ANSWER,
		SEND_SECOND,
		WAIT_SECOND_ANSWER
	};


private:
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

	RobotData _waitAnswer;

	std::atomic_bool _isNeedToWait;

	CirclicState _circlicState;

	/**
	 * \brief Default value for server IP.
	 */
	static const std::string_view _DEFAULT_SERVER_IP;

	/**
	* \brief Default value for sending port.
	 */
	static constexpr int _DEFAULT_SENDING_PORT		= 59002;
													
	/**                                             
	 * \brief Default value for receiving port.     
	 */                                             
	static constexpr int _DEFAULT_RECEIVING_PORT	= 59003;

	/**
	 * \brief Default (beginning) robot position.
	 */
	static const RobotData _DEFAULT_POSITION;


	/**
	 * \brief			Function tries to establishe a connection to a specified socket again.
	 * \param[in] port	Port for reconnection.
	 */
	void		tryReconnect(const int port);

	/**
	 * \brief Main infinite working loop. Network logic to interacte with server.
	 */
	void		waitLoop() override;

	/**
	 * \brief Additional fuction that receives data from server.
	 */
	void		receive();

	/**
	 * \brief			Check connection to robot every time.
	 * \param[in] time	Period time to check.
	 */
	void		checkConnection(const std::atomic_int64_t& time);

	void		circlicProcessing(const RobotData firstPoint, const RobotData secondPoint, 
									const std::size_t numberOfIterations = 1u);

	void		partialProcessing(const RobotData firstPoint, const RobotData secondPoint,
									const std::size_t numberOfSteps = 1u);

	void		stepProcessing(RobotData firstPoint, RobotData secondPoint,
									const std::atomic_int& step, const std::atomic_int& stepCount);

	RobotData	shrinkToFit(const RobotData& firstPoint, const RobotData& secondPoint);


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
						const std::string_view serverIP = "192.168.0.21");

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

	void		circlicMovement(const RobotData& firstPoint, const RobotData& secondPoint, 
								const std::size_t numberOfIterations);

	void		partialMovement(const RobotData& firstPoint, const RobotData& secondPoint,
								const std::size_t numberOfSteps);

	// Friendly swapping fuction.
	template <class T>
	friend void utils::swap(T& first, T& second) noexcept;
};

/*
 * Max:
 * x = 1320
 * y = -400
 * z = 960
 * w = ---
 * p = ---
 * r = ---
 * 
 * Min:
 * x = 830
 * y = 317
 * z = 539
 * w = ---
 * p = ---
 * r = ---
 */

}

#endif // CLIENT_H
