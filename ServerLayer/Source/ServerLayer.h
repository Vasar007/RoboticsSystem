#ifndef SERVER_LAYER
#define SERVER_LAYER

#include <map>
#include <mutex>
#include <optional>

#include "Utilities.h"
#include "WinsockInterface.h"


namespace vasily
{

class ServerLayer : public WinsockInterface
{
public:
	/**
	 * \brief                           Constructor that initializes sockets, connects to server and
	 *                                  creates socket to work with clients.
	 * \param[in] serverSendingPort     Server port to send.
	 * \param[in] serverRecivingPort    Server port to recieve.
	 * \param[in] serverIP              Server IP address for connection.
	 * \param[in] layerPort             Additional port to communicate with clients.
	 * \param[in] backlog               The maximum length of the queue of pending connections.
	 */
	explicit ServerLayer(const int serverSendingPort        = _DEFAULT_SENDING_PORT_TO_SERVER,
						 const int serverRecivingPort       = _DEFAULT_RECEIVING_PORT_FROM_SERVER,
						 const std::string_view serverIP    = _DEFAULT_SERVER_IP,
						 const int layerPort                = _DEFAULT_CLIENT_PORT,
						 const int backlog                  = 10);

	/**
	 * \brief			Deleted copy constructor.
	 * \param[in] other Other client object.
	 */
					ServerLayer(const ServerLayer& other)   = delete;

	/**
	 * \brief			Deleted copy assignment operator.
	 * \param[in] other Other client object.
	 * \return			Returns nothing because it's deleted.
	 */
	ServerLayer&    operator=(const ServerLayer& other)     = delete;

	/**
	 * \brief			Deleted move constructor.
	 * \param[in] other Other client object.
	 */
					ServerLayer(ServerLayer&& other)        = delete;

	/**
	 * \brief			Deleted move assignment operator.
	 * \param[in] other Other client object.
	 * \return			Returns nothing because it's deleted.
	 */
	ServerLayer&    operator=(ServerLayer&& other)          = delete;

	/**
	 * \brief Destructor that closes client socket.
	 */
	virtual         ~ServerLayer() noexcept;

	/**
	 * \brief Main method which starts infinite working loop.
	 */
	void            run() override;

	/**
	 * \brief Process sockets.
	 */
	void            launch() override;

	/**
	 * \brief	Get server IP address.
	 * \return	String which contains current server IP address.
	 */
	std::string     getServerIP() const;

	/**
	 * \brief					Set server IP address.
	 * \param[in] newServerIP	New server IP address as string.
	 */
	void		    setServerIP(const std::string_view newServerIP);

	/**
	 * \brief Additional fuction that receives data from server.
	 */
	void		    receiveFromServer();

	/**
	 * \brief Additional fuction that receives data from clients.
	 */
	void            receiveFromClients();

	/**
	 * \brief				Check if given point is not out of working coordinates.
	 * \param[in] robotData	Point to check.
	 * \return				True if point is correct, false otherwise.
	 */
	bool		checkCoordinates(const RobotData& robotData) const;


protected:
	/**
	 * \brief Receive buffer that is used to keep answers from clients.
	 */
	char		_bufferForClient[_MAXRECV];

	/**
	 * \brief Buffer that is used to keep clients addresses.
	 */
	char		_messageWithIPForClient[_MAXRECV];

	/**
	 * \brief Flag used to define active connection to client.
	 */
	std::atomic_bool _isRunningForClient;

	/**
	 * \brief Variable used to keep server IP address.
	 */
	std::string _serverIP;

	/**
	 * \brief Variable used to keep server port.
	 */
	int			_layerPort;

	/**
	 * \brief The maximum length of the queue of pending connections.
	 */
	int			_backlog;

	/**
	* \brief Connected client socket used to send and receive data.
	*/
	SOCKET		_clientSocket;

	/**
	 * \brief Connected client socket used to send and receive data.
	 */
	SOCKET		_layerSocket;

	/**
	 * \brief Structure used to keep socket address to send and receive data.
	 */
	SOCKADDR_IN	_layerSocketAddress;

	/**
	 * \brief Variable used to keep server port to send.
	 */
	int			_serverSendingPort;

	/**
	 * \brief Variable used to keep server port to recieve.
	 */
	int			_serverReceivingPort;

	/**
	 * \brief Variable used to keep coordinate type from client.
	 */
	std::optional<CoordinateSystem>	_coorninateSystem;

	/**
	 * \brief Last received data from client.
	 */
	RobotData	_lastReceivedPoint;

	/**
	 * \brief Flag used to know if data table successful read or not.
	 */
	bool        _hasReadTable;

	/**
	 * \brief Data table which keeps compliance with distance and time.
	 */
	std::map<long long, std::chrono::milliseconds> _distanceToTimeTable;

	/**
	 * \brief Queue used t keeps received messages frm clients.
	 */
	std::deque<RobotData> _messagesStorage;

	/**
	 * \brief Mutex to lock thread for safety.
	 */
	std::mutex              _mutex;

	/**
	 * \brief Logger used to write received data to file.
	 */
	logger::Logger          _logger;

	/**
	 * \brief Default file name for input.
	 */
	static constexpr char   _DEFAULT_IN_FILE_NAME[]             = "distance_to_time.txt";

	/**
	 * \brief Default file name for output.
	 */
	static constexpr char   _DEFAULT_OUT_FILE_NAME[]            = "out.txt";

	/**
	 * \brief Default value for server IP.
	 */
	static constexpr char	_DEFAULT_SERVER_IP[]                = "192.168.1.21";

	/**
	 * \brief Default value for sending port.
	 */
	static constexpr int	_DEFAULT_SENDING_PORT_TO_SERVER     = 59002;

	/**
	 * \brief Default value for receiving port.
	 */
	static constexpr int	_DEFAULT_RECEIVING_PORT_FROM_SERVER = 59003;

	/**
	 * \brief Constant number of server layer port to send and receive message for clients.
	 */
	static constexpr int	_DEFAULT_CLIENT_PORT                = 8888;

	/**
	 * \brief Constant number of coordinates to check to avoid "magic number".
	 */
	static constexpr std::size_t _MAIN_COORDINATES		        = 3u;

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
	void tryReconnectToServer();

	/**
	 * \brief			Check connection to robot every time.
	 * \param[in] time	Period time to check.
	 */
	void checkConnectionToServer(const long long& time);

	/**
	 * \brief           Calculate duration for currrent movement section.
	 * \param robotData New point of movement.
	 * \return          Approximately duration in milliseconds. 
	 */
	std::chrono::milliseconds calculateDuration(const RobotData& robotData);

	/**
	 * \brief Additional loop which has a handler for connections.
	 */
	void process();

	/**
	 * \brief Wait for clients connections.
	 */
	void waitingForConnections();

	/**
	 * \brief Main infinite working loop. All network logic should be placed here.
	 */
	void waitLoop() override;

	/**
	 * \brief Additional method which contains all connection calls. 
	 */
	void doConnection();


private:
	/**
	 * \brief   Read data from file into container.
	 * \return  True if reading was successful, false otherwise.
	 */
	bool readDataTable();
};

} // namespace vasily

#endif // SERVER_LAYER
