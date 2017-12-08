#ifndef ROBOT_DATA_H
#define ROBOT_DATA_H

#include <array>
#include <sstream>


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
	constexpr	RobotData() noexcept
					:
					mCoordinates{},
					mParameters{ 0, 0, 0 }
				{
				}

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
	constexpr	RobotData(const int x, const int y, const int z, const int w, const int p, 
						  const int r, const int segtime, const int movingType, 
						  const int lastCommand) noexcept
					:
					mCoordinates{ x, y, z, w, p, r },
					mParameters{ segtime, movingType, lastCommand }
				{
				}

	/**
	 * \brief	Convert object to string.
	 * \return	String which contains all data of this object.
	 */
	std::string toString() const;

	/**
	 * \brief	Calculate length of vector which comtains 6 coordinates.
	 * \return	Square root of sum of coordinates squares.
	 */
	double		length() const;

	/**
	 * \brief			Function checks two structures on full equality.
	 * \param[in] other Other object to compare.
	 * \return			True if structures are equal, false otherwise.
	 */
	bool		isEqual(const RobotData& other) const;


	/**
	 * \brief			Function checks coordinate arrays of two structures on equality.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			True if 6 coordinates are equal, false otherwise.
	 */
	friend bool operator ==(const RobotData& lhs, const RobotData& rhs);
				
	/**
	 * \brief			Function checks coordinate arrays of two structures on equality.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			True if 6 coordinates are not equal, false otherwise.
	 */
	friend bool operator !=(const RobotData& lhs, const RobotData& rhs);
				
	/**
	 * \brief			Function compares coordinate arrays of two structures.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			True if lhs object is less than rhs object, false otherwise.
	 */
	friend bool operator <(const RobotData& lhs, const RobotData& rhs);
				
	/**
	 * \brief			Function compares coordinate arrays of two structures.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			True if lhs object is greater than rhs object, false otherwise.
	 */
	friend bool operator >(const RobotData& lhs, const RobotData& rhs);

	/**
	 * \brief			Function subtract coordinate rhs object from lhs object.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			Result of sustraction.
	 */
	friend constexpr RobotData operator -(const RobotData& lhs, const RobotData& rhs)
	{
		RobotData result;
		for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
		{
			result.mCoordinates.at(i) = lhs.mCoordinates.at(i) - rhs.mCoordinates.at(i);
		}

		return result;
	}

	/**
	 * \brief			Function sum coordinate of lhs and rhs objects.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			Result of sum.
	 */
	friend constexpr RobotData operator +(const RobotData& lhs, const RobotData& rhs)
	{
		RobotData result;
		for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
		{
			result.mCoordinates.at(i) = lhs.mCoordinates.at(i) + rhs.mCoordinates.at(i);
		}

		return result;
	}

	/**
	 * \brief			Function sum coordinate of lhs and rhs objects.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			Result of sum puts in lhs object.
	 */
	friend constexpr RobotData& operator +=(RobotData& lhs, const RobotData& rhs)
	{
		for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
		{
			lhs.mCoordinates.at(i) += rhs.mCoordinates.at(i);
		}

		return lhs;
	}

	/**
	 * \brief			Function divides coordinate of lhs and rhs objects.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			Result of division.
	 */
	friend constexpr RobotData operator /(const RobotData& lhs, const int& rhs)
	{
		RobotData result;
		for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
		{
			if (lhs.mCoordinates.at(i) == 0)
			{
				continue;
			}

			result.mCoordinates.at(i) = lhs.mCoordinates.at(i) / rhs;
			if (result.mCoordinates.at(i) == 0)
			{
				result.mCoordinates.at(i) = 1;
			}
		}

		return result;
	}


	/**
	 * \brief			Function divides coordinate of lhs and rhs objects.
	 * \param[in] lhs	Left-hand side object.
	 * \param[in] rhs	Right-hand side object.
	 * \return			Result of division puts in lhs object.
	 */
	friend constexpr RobotData& operator /=(RobotData& lhs, const int& rhs)
	{
		for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
		{
			if (lhs.mCoordinates.at(i) == 0)
			{
				continue;
			}

			lhs.mCoordinates.at(i) = lhs.mCoordinates.at(i) / rhs;
			if (lhs.mCoordinates.at(i) == 0)
			{
				lhs.mCoordinates.at(i) = 1;
			}
		}

		return lhs;
	}

	/**
	 * \brief				Operator overloading for istream.
	 * \param[in] cin		Refrence to the original istream.
	 * \param[in] robotData	Object for records.
	 * \return				A reference to the original istream object.
	 */
	friend constexpr std::istream& operator >>(std::istream& cin, RobotData& robotData)
	{
		for (auto& coordinate : robotData.mCoordinates)
		{
			cin >> coordinate;
		}

		for (auto& parameter : robotData.mParameters)
		{
			cin >> parameter;
		}

		return cin;
	}
};

}

#endif // ROBOT_DATA_H
