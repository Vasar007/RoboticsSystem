#ifndef HANDLER_H
#define HANDLER_H

#include <string>
#include <unordered_map>

#include "Utilities.h"


namespace vasily
{

/**
 * \brief Structure used to contains additional input data from client.
 */
struct ParsedResult
{
    /**
     * \brief Number used to change coordinates.
     */
    int                    coefficient        = 1;

    /**
     * \brief Number of iterations for circlic movement.
     */
    int                    numberOfIterations = 1;

    /**
     * \brief Additional flag to check correctness of coordinates.
     */
    bool                   isCorrect          = true;

    /**
     * \brief List of parsed points.
     */
    std::vector<RobotData> points             = {};
};

/**
 * \brief Additional helper which provides parsing input data from client.
 */
class Handler
{
public:
    /**
     * \brief Array of moves states.
     */
    enum class State
    {
        DEFAULT,

        COORDINATE_TYPE,
        FULL_CONTROL,
        CIRCLIC,
        PARTIAL,
        POSITIONAL,
        HOME,
        FROM_FILE,

        FORWARD,
        BACK,
        
        LEFT,
        RIGHT,
        
        UP,
        DOWN,

        ROLL_PLUS,
        ROLL_MINUS,

        PITHCH_PLUS,
        PITHCH_MINUS,

        YAW_PLUS,
        YAW_MINUS
    };

    /**
     * \brief Input mode.
     */
    enum class Mode
    {
        READING,
        COMMAND
    };

    /**
     * \brief Coordinate name, just to avoid "magic numbers".
     */
    static constexpr std::size_t X = 0;

    /**
     * \brief Coordinate name, just to avoid "magic numbers".
     */
    static constexpr std::size_t Y = 1;
    
    /**
     * \brief Coordinate name, just to avoid "magic numbers".
     */
    static constexpr std::size_t Z = 2;
    
    /**
     * \brief Coordinate name, just to avoid "magic numbers".
     */
    static constexpr std::size_t W = 3;
    
    /**
     * \brief Coordinate name, just to avoid "magic numbers".
     */
    static constexpr std::size_t P = 4;
    
    /**
     * \brief Coordinate name, just to avoid "magic numbers".
     */
    static constexpr std::size_t R = 5;


    /**
     * \brief Default constructor.
     */
    Handler();

    /**
     * \brief                Get command and change current state if its valid.
     * \param[in] command    String for parsing.
     * \param[out] robotData Robot data which contains coordinates to change.
     */
    void appendCommand(const std::string_view command, RobotData& robotData);

    /**
     * \brief                Get input data and change current state if its valid.
     * \param[in] data       String for parsing.
     * \param[out] robotData Robot data which contains coordinates to change.
     */
    void parseRawData(const std::string& data, RobotData& robotData);

    /**
     * \brief  Get current moving coefficient.
     * \return Current moving coefficient.
     */
    int getCoefficient() const noexcept;

    /**
     * \brief                 Set moving coefficient.
     * \param[in] coefficient New value to set.
     */
    void setCoefficient(const int coefficient) noexcept;

    /**
     * \brief  Get handle state.
     * \return Current handle state.
     */
    State getCurrentState() const noexcept;

    /**
     * \brief	Get client mode.
     * \return	Current client mode.
     */
    Mode getCurrentMode() const noexcept;

    /**
     * \brief          Set mode to client.
     * \param[in] mode New value to set.
     */
    void setMode(const Mode mode) noexcept;

    /**
    * \brief  Get coordinate type.
    * \return Current coordinate type to work with robot.
    */
    CoordinateSystem getCoordinateSystem() const noexcept;

    /**
    * \brief                       Set coordinate type to work with robot.
    * \param[in] coordninateSystem New value to set.
    */
    void setCoordinateSystem(const CoordinateSystem coordninateSystem) noexcept;

    /**
     * \brief  Get parsed result after command.
     * \return Current parsed result after command.
     */
    ParsedResult getParsedResult() const noexcept;


private:
    /**
     * \brief Handler state.
     */
    State                                  _state;

    /**
     * \brief Client input mode.
     */
    Mode                                   _mode;

    /**
     * \brief Coordinate type to work.
     */
    CoordinateSystem                       _coorninateSystem;

    /**
     * \brief Data buffer from client.
     */
    std::string                            _data;

    /**
     * \brief Moving coefficient which we use to change the coordinates.
     */
    int										_coefficient;

    /**
     * \brief Parsed data structure which build from input data after command literal.
     */
    ParsedResult                           _parsedResult;

    /**
     * \brief Associative array which contains keyboard button and move state command.
     */
    std::unordered_map<std::string, State> _mapping;

    /**
    * \brief Implementation of type-safe output printer.
    */
    printer::Printer&                       _printer = printer::Printer::getInstance();


    /**
     * \brief             Parse input string into command.
     * \param[in] command String for parsing.
     * \return            Move state command.
     */
    State parseCommand(const std::string_view command);

    /**
     * \brief  Parse input string after command literal.
     * \return Parsed structure which contains all stuf for interact with robot.
     */
    ParsedResult parseDataAfterCommand();

    /**
     * \brief            Check if letter contains command to change current mode.
     * \param[in] letter String for parsing.
     * \return           True if mode is changed, false otherwise.
     */
    bool checkChangingMode(const std::string_view letter);

    /**
     * \brief            Check if letter contains command to change current coordinate
     *                   system.
     * \param[in] letter String for parsing.
     * \return           True if system is changed, false otherwise.
     */
    bool checkChangingCoordinateSysytem(const std::string_view letter);
};

} // namespace vasily

#endif // HANDLER_H
