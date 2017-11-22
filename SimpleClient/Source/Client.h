#ifndef CLIENT_H
#define CLIENT_H
#pragma once

#include "WinsockInterface.h"
#include "RobotData.h"
#include "Utility.h"


namespace vasily
{

/**
 * \brief Typical realization of client used for interaction with robot server.
 */
class Client : public WinsockInterface
{
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


public:
	/**
	 * \brief					Constructor that initializes sockets and connects to server.
	 * \param[in] serverPort	Server port for connection.
	 * \param[in] serverIP		Server IP address for connection.
	 */
	explicit	Client(const int serverPort, const std::string_view serverIP);

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
	void		setServerIP(std::string_view newServerIP);

	/**
	 * \brief Main method which starts infinite working loop.
	 */
	void		run() override;

	/**
	 * \brief Fuction processes sockets (call 'connect').
	 */
	void		launch() override;

	// Friendly swapping fuction.
	template <class T>
	friend void utils::swap(T& first, T& second) noexcept;
};

}

#endif // CLIENT_H
