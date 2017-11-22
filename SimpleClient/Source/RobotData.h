#ifndef ROBOT_DATA_H
#define ROBOT_DATA_H
#pragma once

#include <array>
#include <string_view>


namespace vasily
{

/**
 * \brief POD(Plain-Old-Data) which contains values and parameteres to exchanging.
 */
struct RobotData
{
	/**
	 * \brief Constant number of values to avoid "magic number".
	 */
	static constexpr int NUMBER_OF_VALUES	  = 6;
	/**
	 * \brief Constant number of parameters to avoid "magic number".
	 */
	static constexpr int NUMBER_OF_PARAMETERS = 3;

	/**
	 * \brief Array which contains only coordinates or degrees.
	 */
	std::array<int, NUMBER_OF_VALUES>	  mValues;
	/**
	 * \brief Array which contains some parameters.
	 */
	std::array<int, NUMBER_OF_PARAMETERS> mParameters;


	/**
	 * \brief Default constructor.
	 */
				RobotData();

	/**
	 * \brief	Convert object to string.
	 * \return	String which contains all data of this object.
	 */
	std::string toString() const;

	/**
	 * \brief			Operator overloading for istream.
	 * \param cin		Refrence to the original istream.
	 * \param robotData	Object for records.
	 * \return			Aa reference to the original istream object.
	 */
	friend std::istream& operator>>(std::istream& cin, RobotData& robotData);
};

}

#endif // ROBOT_DATA_H
