#ifndef SERVER_H
#define SERVER_H

#include <atomic>

#include "WinsockInterface.h"
#include "Utilities.h"


namespace vasily
{

/**
 * \brief Pretty simple imitator FanucM20iA.
 */
class ServerImitator : public WinsockInterface
{
private:
	/**
	 * \brief Variable used to keep sending port.
	 */
	int					_sendingPort;
					
	/**
	 * \brief Variable used to keep reciving port.
	 */
	int					_receivingPort;
					
	/**
	 * \brief The maximum length of the queue of pending connections.
	 */
	int					_backlog;

	/**
	 * \brief Connected client socket used to send data.
	 */
	SOCKET				_clientSocketSend;

	/**
	 * \brief Connected client socket used to receive data.
	 */
	SOCKET				_clientSocketReceive;

	/**
	 * \brief Additional flag used to define coordinate type from client data.
	 */
	std::atomic_bool	_hasGotCoordSystem;
	

	/**
	 * \brief Main infinite working loop. Network logic to interacte with clients.
	 */
	void			waitLoop() override;

	/**
	 * \brief			Additional loop which has a handler for connections.
	 * \param[out] flag Flag used to know when client connected.
	 */
	void			process(std::atomic_bool& flag);

	/**
	 * \brief			Function waits for clients connections.
	 * \param[out] flag	Flag used to know when client connected.
	 */
	void			waitingForConnections(std::atomic_bool& flag);


public:
	/**
	 * \brief					Constructor which initializes sockets and bindes ports to them.
	 * \param[in] sendingPort	Port for connection.
	 * \param[in] recivingPort	Port for connection.
	 * \param[in] backlog		Number of connections allowed on the incoming queue.
	 */
	explicit		ServerImitator(const int sendingPort, const int recivingPort, 
								   const int backlog = 10);

	/**
	 * \brief Default destructor.
	 */
					~ServerImitator()							= default;

	/**
	 * \brief			Deleted copy constructor.
	 * \param[in] other Other client object.
	 */
					ServerImitator(const ServerImitator& other) = delete;

	/**
	 * \brief			Deleted copy assignment operator.
	 * \param[in] other Other client object.
	 * \return			Returns nothing because it's deleted.
	 */
	ServerImitator&	operator=(const ServerImitator& other)		= delete;

	/**
	 * \brief				Move constructor.
	 * \param[out] other	Other client object.
	 */
					ServerImitator(ServerImitator&& other) noexcept;

	/**
	 * \brief				Move assignment operator.
	 * \param[out] other	Other client object.
	 * \return				Returns an object with all moved data.
	 */
	ServerImitator&	operator=(ServerImitator&& other) noexcept;

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

}

#endif // SERVER_H
