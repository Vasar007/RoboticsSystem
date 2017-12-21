#ifndef ROBOT_DATA_H
#define ROBOT_DATA_H

#include <array>


namespace vasily
{

/**
 * \brief Simple structure which contains values and parameteres to exchanging.
 */
struct RobotData
{
	/**
	 * \brief Constant number of values to avoid "magic number".
	 */
	static constexpr std::size_t NUMBER_OF_COORDINATES	= 6u;
	
	/**
	 * \brief Constant number of parameters to avoid "magic number".
	 */
	static constexpr std::size_t NUMBER_OF_PARAMETERS	= 3u;

	/**
	 * \brief Default coordinates.
	 */
	static constexpr std::array<int, NUMBER_OF_COORDINATES>	DEFAULT_CORDINATES
															{ 985'000, 0, 940'000, -180'000, 0, 0 };

	/**
	 * \brief Default robot parameters.
	 */
	static constexpr std::array<int, NUMBER_OF_PARAMETERS>	DEFAULT_PARAMETERS
															{ 1, 2, 0 };

	/**
	 * \brief Array which contains only coordinates or degrees.
	 */
	std::array<int, NUMBER_OF_COORDINATES>	coordinates;

	/**
	 * \brief	Array which contains some parameters.
	 * \details	Segtime (!= 0), type of moving(0 - FINE/ 2 - NOVAR+NODECEL), start/stop (0/1)
	 */
	std::array<int, NUMBER_OF_PARAMETERS>	parameters;


	/**
	 * \brief Default constructor for default position.
	 */
	constexpr	RobotData() noexcept
					: coordinates(DEFAULT_CORDINATES),
					  parameters(DEFAULT_PARAMETERS)
				{
				}

	/**
	 * \brief					Constructor for creating structure from 9 numbers.
	 * \param[in] x				1st coordinate.
	 * \param[in] y				2nd coordinate.
	 * \param[in] z				3rd coordinate.
	 * \param[in] w				4th coordinate.
	 * \param[in] p				5th coordinate.
	 * \param[in] r				6th coordinate.
	 * \param[in] segtime		Segtime.
	 * \param[in] movingType	Type of moving: Nodecel or Fine.
	 * \param[in] lastCommand	Special command for robot.
	 */
	constexpr	RobotData(const int x, const int y, const int z, const int w, const int p, 
						  const int r, const int segtime, const int movingType, 
						  const int lastCommand) noexcept
					: coordinates{ x, y, z, w, p, r },
					  parameters{ segtime, movingType, lastCommand }
				{
				}

	/**
	 * \brief                       Constructor for creating structure from two arrays.
	 * \param[in] initCoordinates   Array that contains coordinates.
	 * \param[in] initParameters    Array that contains parameters.
	 */
	constexpr	RobotData(const std::array<int, NUMBER_OF_COORDINATES>& initCoordinates,
						  const std::array<int, NUMBER_OF_PARAMETERS>& initParameters) noexcept
					: coordinates(initCoordinates),
					  parameters(initParameters)
	{
	}

	/**
	 * \brief Default destructor.
	 */
				~RobotData() noexcept                           = default;

	/**
	 * \brief			Default copy constructor.
	 * \param[in] other Other object.
	 */
	constexpr   RobotData(const RobotData& other)               = default;

	/**
	 * \brief			Default copy assignment operator.
	 * \param[in] other Other object.
	 * \return			Return copied structure.
	 */
	constexpr RobotData& operator=(const RobotData& other)      = default;

	/**
	 * \brief				Default move constructor.
	 * \param[out] other	Other object.
	 */
	constexpr   RobotData(RobotData&& other) noexcept           = default;

	/**
	 * \brief				Default move assignment operator.
	 * \param[out] other	Other object.
	 * \return				Return moved structure.
	 */
	constexpr RobotData& operator=(RobotData&& other) noexcept  = default;


	/**
	 * \brief	Convert object to string.
	 * \return	String which contains all data of this object.
	 */
	std::string toString() const;

	/**
	 * \brief	Calculate length of vector which contains 6 coordinates.
	 * \detail  Fucntion uses only first 3 coordinates!
	 * \return	Square root of sum of coordinates squares.
	 */
	double		length() const;

	/**
	 * \brief			Check two structures on full equality.
	 * \param[in] other Other object to compare.
	 * \return			True if structures are equal, false otherwise.
	 */
	bool		isEqual(const RobotData& other) const;

	/**
	 * \brief Reset coordinates and parameters values to default.
	 */
	void        returnToDefault();


	/**
	 * \brief			Check coordinate arrays of two structures on equality.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			True if 6 coordinates are equal, false otherwise.
	 */
	friend bool operator ==(const RobotData& lhs, const RobotData& rhs);
				
	/**
	 * \brief			Check coordinate arrays of two structures on equality.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			True if 6 coordinates are not equal, false otherwise.
	 */
	friend bool operator !=(const RobotData& lhs, const RobotData& rhs);
				
	/**
	 * \brief			Compare coordinate arrays of two structures.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			True if lhs object is less than rhs object, false otherwise.
	 */
	friend bool operator <(const RobotData& lhs, const RobotData& rhs);
				
	/**
	 * \brief			Compare coordinate arrays of two structures.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			True if lhs object is greater than rhs object, false otherwise.
	 */
	friend bool operator >(const RobotData& lhs, const RobotData& rhs);

	/**
	 * \brief			Subtract coordinate rhs object from lhs object.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			Result of sustraction.
	 */
	friend RobotData operator -(const RobotData& lhs, const RobotData& rhs);

	/**
	 * \brief			Sum coordinate of lhs and rhs objects.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			Result of sum.
	 */
	friend RobotData operator +(const RobotData& lhs, const RobotData& rhs);

	/**
	 * \brief			Sum coordinate of lhs and rhs objects.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			Result of sum puts in lhs object.
	 */
	friend RobotData& operator +=(RobotData& lhs, const RobotData& rhs);

	/**
	 * \brief			Divide coordinate of lhs and rhs objects.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			Result of division.
	 */
	friend RobotData operator /(const RobotData& lhs, const int& rhs);

	/**
	 * \brief			Divide coordinate of lhs and rhs objects.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			Result of division puts in lhs object.
	 */
	friend RobotData& operator /=(RobotData& lhs, const int& rhs);

	/**
	 * \brief				Operator overloading for istream.
	 * \param[in] cin		Refrence to the original istream.
	 * \param[in] robotData	Object for records.
	 * \return				A reference to the original istream object.
	 */
	friend std::istream& operator >>(std::istream& cin, RobotData& robotData);
};

}

#endif // ROBOT_DATA_H
