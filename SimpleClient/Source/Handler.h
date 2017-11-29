#ifndef HANDLER_H
#define HANDLER_H

#include <string>
#include <unordered_map>

#include "RobotData.h"


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
	int			mCoefficient		= 1;

	/**
	 * \brief Number of iterations for circlic movement.
	 */
	std::size_t	mNumberOfIterations	= 1u;

	bool		mIsCorrect			= true;

	/**
	 * \brief First point of interval.
	 */
	RobotData	mFirstPoint			= {};

	/**
	 * \brief Second point of interval.
	 */
	RobotData	mSecondPoint		= {};
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
		HOME,

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
	 * \brief Array of coordinate type to work with robot.
	 */
	enum class CoorninateSystem
	{
		JOINT	= 1,
		WORLD	= 2
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
	 * \brief Coordinates names, just to avoid "magic numbers".
	 */
	enum Coordinate
	{
		X = 0,
		Y = 1,
		Z = 2,
		W = 3,
		P = 4,
		R = 5
	};


private:
	/**
	 * \brief Handler state.
	 */
	State									_state;

	/**
	 * \brief Client input mode.
	 */
	Mode									_mode;

	/**
	 * \brief Coordinate type for working.
	 */
	CoorninateSystem						_coorninateSystem;

	/**
	 * \brief Data buffer from client.
	 */
	std::string								_data;

	/**
	 * \brief Moving coefficient which we use to change the coordinates.
	 */
	int										_coefficient;

	/**
	 * \brief Number of iterations for circlic movement.
	 */
	std::size_t								_numberOfIterations;

	/**
	 * \brief Parsed data structure which build from input data after command literal.
	 */
	ParsedResult							_parsedResult;

	/**
	 * \brief Associative array which contains keyboard button and move state command.
	 */
	std::unordered_map<std::string, State>	_mapping;


	/**
	 * \brief				Function parses input string into command.
	 * \param[in] command	String for parsing.
	 * \return				Move state command.
	 */
	State parseCommand(const std::string_view command);

	/**
	 * \brief	Function parses input string after command literal.
	 * \return	Parsed structure which contains all stuf for interact with robot.
	 */
	ParsedResult parseDataAfterCommand();

	/**
	 * \brief				Fuction checks if letter contains command to change current mode.
	 * \param[in] letter	String for parsing.
	 * \return				True if mode is changed, false otherwise.
	 */
	bool checkChangingMode(const std::string& letter);

	/**
	 * \brief				Fuction checks if letter contains command to change current coordinate
	 *						system.
	 * \param[in] letter	String for parsing.
	 * \return				True if system is changed, false otherwise.
	 */
	bool checkChangingCoordinateSysytem(const std::string& letter);


public:
	/**
	 * \brief Default constructor.
	 */
	Handler();

	/**
	 * \brief					Gets command and changes current state if its valid.
	 * \param[in] command		String for parsing.
	 * \param[out] robotData	Robot data which contains coordinates to change.
	 */
	void appendCommand(const std::string_view command, RobotData& robotData);

	/**
	 * \brief					Gets input data and changes current state if its valid.
	 * \param[in] data			String for parsing.
	 * \param[out] robotData	Robot data which contains coordinates to change.
	 */
	void parseRawData(const std::string& data, RobotData& robotData);

	/**
	 * \brief	Function returns current moving coefficient.
	 * \return	Current moving coefficient.
	 */
	int getCoefficient() const;

	/**
	 * \brief					Functions sets moving coefficient.
	 * \param[in] coefficient	New value to set.
	 */
	void setCoefficient(const int coefficient);

	/**
	 * \brief	Function returns handle state.
	 * \return	Current handle state.
	 */
	State getCurrentState() const;

	/**
	 * \brief	Function returns client mode.
	 * \return	Current client mode.
	 */
	Mode getCurrentMode() const;

	/**
	 * \brief			Function sets mode to client.
	 * \param[in] mode	New value to set.
	 */
	void setMode(const Mode mode);

	/**
	* \brief	Function returns coordinate type.
	* \return	Current coordinate type to work with robot.
	*/
	CoorninateSystem getCoordinateSystem() const;

	/**
	* \brief						Function sets coordinate type to work with robot.
	* \param[in] coorninateSystem	New value to set.
	*/
	void setCoordinateSystem(const CoorninateSystem coorninateSystem);

	/**
	 * \brief	Function returns number of iterations.
	 * \return	Current number of iterations.
	 */
	int getNumberOfIterations() const;

	/**
	 * \brief	Function returns parsed result after command.
	 * \return	Current parsed result after command.
	 */
	ParsedResult getParsedResult() const;
};

}

#endif // HANDLER_H
