#ifndef MOD_CLIENT_H
#define MOD_CLIENT_H

#include <mutex>

#include "Client.h"


namespace clientTests
{

class ModClient : public vasily::Client
{
public:
	/**
	 * \brief Constant number of server port.
	 */
	static constexpr int	SERVER_PORT				= 9997;

	/**
	 * \brief Constant number of server port to send.
	 */
	static constexpr int	SERVER_PORT_SENDING		= 9999;

	/**
	 * \brief Constant number of server port to receive.
	 */
	static constexpr int	SERVER_PORT_RECEIVING	= 9998;

	/**
	 * \brief Constant number of server IP.
	 */
	static constexpr char	SERVER_IP[]				= "192.168.0.102";

	/**
	 * \brief Mutex to lock thread for safety.
	 */
	std::mutex					mMutex;

	/**
	 * \brief Additional flag used to define end of receiving method.
	 */
	std::atomic_bool			mHasFinished;

	/**
	 * \brief Array of sent data to server to check.
	 */
	std::vector<std::string>	mStorage;


	/**
	 * \brief						Constructor that initializes sockets and connects to server.
	 * \param[in] serverPortSending	Server port to send.
	 * \param[in] serverReceiving	Server port to recieve.
	 * \param[in] serverIP			Server IP address for connection.
	 */
	explicit	ModClient(const int serverPortSending = SERVER_PORT_SENDING,
						  const int serverReceiving = SERVER_PORT_RECEIVING,
						  const std::string_view serverIP = SERVER_IP);

	/**
	 * \brief					Function receives data from receiving socket.
	 * \details					Create additional thread to receive data from sercer.
	 * \param[in] numberOfTimes Number of times to allow connections.
	 */
	void		receiveDataNTimes(const std::size_t numberOfTimes);

	/**
	 * \brief				Function checks coordinates and if it's right sends to robot.
	 * \param[in] robotData Point to send.
	 */
	void		sendCoordinatesMod(const vasily::RobotData& robotData);

	/**
	 * \brief							Function launches thread for circlic processing and
	 *									forwards parameters.
	 * \details							Now function works only with 2 points!
	 * \param[in] firstPoint			First point to send and in which robot should return.
	 * \param[in] secondPoint			Second point for circlic movement.
	 * \param[in] numberOfIterations	Number of iterations in circlic movement.
	 */
	void		circlicMovementMod(const vasily::RobotData& firstPoint, 
								   const vasily::RobotData& secondPoint,
								   const std::size_t numberOfIterations);


	/**
	 * \brief					Function launches thread for partial processing and
	 *							forwards parameters.
	 * \details					Now function works only with 2 points!
	 * \param[in] firstPoint	Start point.
	 * \param[in] secondPoint	End point.
	 * \param[in] numberOfSteps	Number of steps for which robot should move from start to end point.
	 */
	void		partialMovementMod(const vasily::RobotData& firstPoint, 
								   const vasily::RobotData& secondPoint,
								   const std::size_t numberOfSteps);

	/**
	 * \brief					Spawn new thread to receive data from client.
	 * \param[in] numberOfTimes Number of times to allow connections.
	 * \return					Created thread.
	 */
	std::thread	spawn(const std::size_t numberOfTimes);
};

}

#endif // MOD_CLIENT_H