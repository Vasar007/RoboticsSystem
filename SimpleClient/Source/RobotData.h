#ifndef ROBOT_DATA_H
#define ROBOT_DATA_H

#include <array>
#include <string_view>


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
	 * \brief Array which contains only coordinates or degrees.
	 */
	std::array<int, NUMBER_OF_COORDINATES>	mCoordinates;

	/**
	 * \brief	Array which contains some parameters.
	 * \details	Segtime (!= 0), type of moving(0 - FINE/ 2 - NOVAR+NODECEL), start/stop (0/1)
	 */
	std::array<int, NUMBER_OF_PARAMETERS>	mParameters;


	/**
	 * \brief Default constructor.
	 */
				RobotData();

	/**
	 * \brief					Constructor with parameters.
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
				RobotData(const int x, const int y, const int z, const int w, const int p, 
							const int r, const int segtime, const int movingType, 
							const int lastCommand);

	/**
	 * \brief	Convert object to string.
	 * \return	String which contains all data of this object.
	 */
	std::string toString() const;

	double length() const;

	bool isEqual(const RobotData& other) const;

	friend bool operator ==(const RobotData& lhs, const RobotData& rhs);
				
	friend bool operator !=(const RobotData& lhs, const RobotData& rhs);
				
	friend bool operator <(const RobotData& lhs, const RobotData& rhs);
				
	friend bool operator >(const RobotData& lhs, const RobotData& rhs);

	friend RobotData operator -(const RobotData& lhs, const RobotData& rhs);

	friend RobotData operator +(const RobotData& lhs, const RobotData& rhs);

	friend RobotData& operator +=(RobotData& lhs, const RobotData& rhs);

	friend RobotData operator /(const RobotData& lhs, const int& rhs);

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
