#ifndef CLIENT_H
#define CLIENT_H
#pragma once

#include "WinsockInterface.h"
#include "RobotData.h"


/**
 * \brief Typical realization of client for interacting with connected to robot server.
 */
class Client : public WinsockInterface
{
	private:
		/**
		 * \brief Structure which keeping data for interacting with robot.
		 */
		RobotData	_robotData;

		/**
		 * \brief Variable for keeping server IP address.
		 */
		std::string _serverIP;


		/**
		 * \brief	   Function tries to establishe a connection to a specified socket again.
		 * \param port Port for reconnection.
		 */
		void		tryReconnect(const int port);

		/**
		 * \brief Main infinite working loop. Network logic to interacte with server.
		 */
		void		waitLoop() override;


	public:
		/**
		 * \brief			 Constructor that initializes sockets and connects to server.
		 * \param serverPort Server port for connection.
		 * \param serverIP	 Server IP address for connection.
		 */
		explicit	Client(const int serverPort, const std::string& serverIP);

		/**
		* \brief  Function returns server IP address.
		* \return String which contains current server IP address.
		*/
		std::string getServerIP() const;

		/**
		 * \brief			  Function sets server IP address.
		 * \param newServerIP New server IP address as string.
		 */
		void		setServerIP(const std::string& newServerIP);

		/**
		 * \brief				   Function sets timeout for socket.
		 * \param socketForSetting A descriptor identifying a socket.
		 * \param seconds		   Time interval, in seconds.
		 * \param microseconds	   Time interval, in microseconds.
		 */
		void		setTimeout(const SOCKET& socketForSetting, 
							   long seconds, long microseconds) const;

		/**
		* \brief Main method which starts infinite working loop.
		*/
		void		run() override;
};

#endif // CLIENT_H
