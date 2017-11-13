#ifndef SERVER_H
#define SERVER_H
#pragma once

#include "WinsockInterface.h"


/**
 * \brief Pretty simple imitator FanucM20iA.
 */
class ServerImitator : public WinsockInterface
{
	private:
		/**
		 * \brief Maximum number of connectied clients.
		 */
		const int		 _MAX_CLIENTS = 32;

		/**
		 * \brief Set of socket descriptors.
		 */
		FD_SET			 _readfds;

		/**
		 * \brief Array which contains all client sockets.
		 */
		std::vector<int> _clientSockets;


		/**
		 * \brief Main infinite working loop. Network logic to interacte with clients.
		 */
		void			waitLoop() override;


	public:
		/**
		 * \brief			   Constructor which initializes sockets and bindes ports to them.
		 * \param sendingPort  Port for connection.
		 * \param recivingPort Port for connection.
		 * \param backlog	   Number of connections allowed on the incoming queue.
		 */
		explicit		ServerImitator(const int sendingPort, const int recivingPort, 
									   const int backlog = 10);

		/**
		 * \brief	   Function parses input data (from clients).
		 * \param data Data for parsing.
		 * \return	   Deserialized data if parsed successful, empty string otherwise.
		 */
		std::string		parseData(std::string& data) const;

		/**
		 * \brief Main method which starts infinite working loop.
		 */
		void			run() override;
};

#endif // SERVER_H
