#ifndef ROBOT_IMITATOR_H
#define ROBOT_IMITATOR_H

#include <chrono>
#include <optional>

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "Utilities.h"


namespace vasily
{

/**
 * \brief Pretty simple imitator Fanuc M-20iA.
 */
class RobotImitator : public QObject
{
    Q_OBJECT
public:
    /**
     * \brief Array of constant to get parameters from config.
     */
    enum class Param : std::size_t
    {
        DEFAULT_IN_FILE_NAME,
        DEFAULT_OUT_FILE_NAME
    };

    /**
     * \brief   Variable used to keep all default parameters and constants.
     * \details Using std::string instead of std::string_view because Logger constructor needs only
     *          std::string because of std::istream and std::ostream.
     */
    static const config::Config<std::string, std::string> CONFIG;

    /**
     * \brief                  Constructor which initializes sockets and bindes ports to
     *                         them.
     * \param[in] recivingPort Port for connection.
     * \param[in] sendingPort  Port for connection.
     * \param[in] parent       The necessary data for Qt.
     */
    explicit		RobotImitator(const int recivingPort, const int sendingPort,
                                  QObject* parent = nullptr);

    /**
     * \brief Default destructor.
     */
    virtual         ~RobotImitator() = default;

    /**
     * \brief           Deleted copy constructor.
     * \param[in] other Other client object.
     */
    RobotImitator(const RobotImitator& other) = delete;

    /**
     * \brief           Deleted copy assignment operator.
     * \param[in] other Other client object.
     * \return          Returns nothing because it's deleted.
     */
    RobotImitator&	operator=(const RobotImitator& other) = delete;

    /**
     * \brief            Deleted move constructor.
     * \param[out] other Other client object.
     */
    RobotImitator(RobotImitator&& other) = delete;

    /**
     * \brief            Deleted move assignment operator.
     * \param[out] other Other client object.
     * \return           Returns nothing because it's deleted.
     */
    RobotImitator&	operator=(RobotImitator&& other) = delete;

    /**
     * \brief Fuction processes sockets (call 'bind' and 'listen').
     */
    void			launch() const;


 private slots:
    /**
     * \brief Process new client connection to receiving socket.
     */
    void slotNewConnectionOnReceive();
    
    /**
     * \brief Process new client connection to sending socket.
     */
    void slotNewConnectionOnSend();

    /**
     * \brief Receive data from client
     */
    void slotReadFromClient();
    
    /**
     * \brief Process client disconnection from receiving socket.
     */
    void slotClientDisconnectedOnReceive();

    /**
     * \brief Process client disconnection from sending socket.
     */
    void slotClientDisconnectedOnSend() const;


protected:
    /**
     * \brief Implementation of type-safe output printer.
     */
    printer::Printer&               _printer = printer::Printer::getInstance();

    /**
     * \brief Variable used to keep reciving port.
     */
    int                             _receivingPort;

    /**
     * \brief Connected socket used to receive data.
     */
    std::unique_ptr<QTcpServer>	    _receivingSocket;

    /**
     * \brief Pointer to socket used to work with clients.
     */
    QTcpSocket*                     _clientSendingSocket;

    /**
    * \brief Variable used to keep sending port.
    */
    int                             _sendingPort;

    /**
     * \brief Connected socket used to send data.
     */
    std::unique_ptr<QTcpServer>     _sendingSocket;

    /**
     * \brief Pointer to socket used to work with clients.
     */
    QTcpSocket*                     _clientReceivingSocket;

    /**
     * \brief Variable used to keep coordinate type from client.
     */
    std::optional<CoordinateSystem>	_coorninateSystem;

    /**
     * \brief Logger used to write received data to file.
     */
    logger::Logger                  _logger;

    /**
     * \brief Last received data from client.
     */
    RobotData                       _lastReceivedData;


    /**
    * \brief               Calculate duration for currrent movement section.
    * \details             Used to calculate duration for sleeping before sending answer to client.
    * \param[in] robotData New point of movement.
    * \return              Approximately duration in milliseconds.
    */
    std::chrono::milliseconds calculateDuration(const RobotData& robotData);
};

} // namespace vasily

#endif // ROBOT_IMITATOR_H
