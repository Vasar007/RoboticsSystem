#ifndef SERVER_H
#define SERVER_H
#pragma once

#include "WinsockInterface.h"
#include "Utility.h"


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
		 * \brief Variable used to keep sending port.
		 */
		int				_sendingPort;
						
		/**
		 * \brief Variable used to keep reciving port.
		 */
		int				_receivingPort;
						
		/**
		 * \brief The maximum length of the queue of pending connections.
		 */
		int				_backlog;

		/**
		 * \brief Connected client socket used to send data.
		 */
		SOCKET			_clientSocketSend;

		/**
		 * \brief Connected client socket used to receive data.
		 */
		SOCKET			_clientSocketReceive;
		

		/**
		 * \brief Main infinite working loop. Network logic to interacte with clients.
		 */
		void			waitLoop() override;

		/**
		 * \brief Additional loop which has a handler for connections.
		 */
		void			process(bool& flag);


	public:
		/**
		 * \brief				Constructor which initializes sockets and bindes ports to them.
		 * \param sendingPort	Port for connection.
		 * \param recivingPort	Port for connection.
		 * \param backlog		Number of connections allowed on the incoming queue.
		 */
		explicit		ServerImitator(const int sendingPort, const int recivingPort, 
									   const int backlog = 10);

		/**
		 * \brief Default destructor.
		 */
						~ServerImitator()							= default;

		/**
	 	 * \brief		Deleted copy constructor.
		 * \param other Other client object.
		 */
						ServerImitator(const ServerImitator& other) = delete;

		/**
		 * \brief		Deleted copy assignment operator.
		 * \param other Other client object.
		 * \return		Returns nothing because it's deleted.
		 */
		ServerImitator&	operator=(const ServerImitator& other)		= delete;

		/**
		 * \brief		Move constructor.
		 * \param other Other client object.
		 */
						ServerImitator(ServerImitator&& other) noexcept;

		/**
		 * \brief		Move assignment operator.
		 * \param other Other client object.
		 * \return		Returns an object with all moved data.
		 */
		ServerImitator&	operator=(ServerImitator&& other) noexcept;

		/**
		 * \brief		Function parses input data (from clients).
		 * \param data	Data for parsing.
		 * \return		Deserialized data if parsed successful, empty string otherwise.
		 */
		std::string		parseData(const std::string& data) const;

		/**
		 * \brief Main method which starts infinite working loop.
		 */
		void			run() override;

		/**
		 * \brief Fuction processes sockets (call 'bind' and 'listen').
		 */
		void			launch() override;

		// Friendly swapping fuction.
		template <class T>
		friend void utils::swap(T& first, T& second) noexcept;
};

#endif // SERVER_H
