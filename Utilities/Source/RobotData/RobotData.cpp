#include <sstream>
#include <iostream>
#include <cassert>
#include <iterator>

#include "RobotData.h"


namespace vasily
{

std::string RobotData::toString() const
{
	std::stringstream stringStream;

	std::copy(coordinates.begin(), coordinates.end(), 
			  std::ostream_iterator<int>(stringStream, " "));

	std::copy(parameters.begin(), parameters.end(),
			  std::ostream_iterator<int>(stringStream, " "));
	
	return stringStream.str();
}

double RobotData::length() const
{
	double result = 0;

	for (std::size_t i = 0u; i < 3u; ++i)
	{
		double temp = coordinates.at(i) / 10'000;
		temp *= coordinates.at(i) / 10'000;
		result += temp;

	}

	return std::sqrt(result);
}

bool RobotData::isEqual(const RobotData& other) const
{
	return this->coordinates == other.coordinates && this->parameters == other.parameters;
}

void RobotData::returnToDefault()
{
	coordinates = DEFAULT_CORDINATES;
	parameters  = DEFAULT_PARAMETERS;
}

bool operator ==(const RobotData& lhs, const RobotData& rhs)
{
	return lhs.coordinates == rhs.coordinates;
}

bool operator !=(const RobotData& lhs, const RobotData& rhs)
{
	return lhs.coordinates != rhs.coordinates;
}

bool operator <(const RobotData& lhs, const RobotData& rhs)
{
	return lhs.coordinates < rhs.coordinates;
}

bool operator >(const RobotData& lhs, const RobotData& rhs)
{
	return lhs.coordinates > rhs.coordinates;
}

RobotData operator -(const RobotData& lhs, const RobotData& rhs)
{
	RobotData result;
	for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
	{
		result.coordinates.at(i) = lhs.coordinates.at(i) - rhs.coordinates.at(i);
	}

	return result;
}

RobotData operator +(const RobotData& lhs, const RobotData& rhs)
{
	RobotData result;
	for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
	{
		result.coordinates.at(i) = lhs.coordinates.at(i) + rhs.coordinates.at(i);
	}

	return result;
}

RobotData& operator +=(RobotData& lhs, const RobotData& rhs)
{
	for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
	{
		lhs.coordinates.at(i) += rhs.coordinates.at(i);
	}

	return lhs;
}

RobotData operator /(const RobotData& lhs, const int& rhs)
{
	assert(rhs != 0);

	RobotData result;
	for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
	{
		if (lhs.coordinates.at(i) == 0)
		{
			continue;
		}

		result.coordinates.at(i) = lhs.coordinates.at(i) / rhs;
		if (result.coordinates.at(i) == 0)
		{
			result.coordinates.at(i) = 1;
		}
	}

	return result;
}

RobotData& operator /=(RobotData& lhs, const int& rhs)
{
	assert(rhs != 0);

	for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
	{
		if (lhs.coordinates.at(i) == 0)
		{
			continue;
		}

		lhs.coordinates.at(i) = lhs.coordinates.at(i) / rhs;
		if (lhs.coordinates.at(i) == 0)
		{
			lhs.coordinates.at(i) = 1;
		}
	}

	return lhs;
}

std::istream& operator >>(std::istream& cin, RobotData& robotData)
{
	for (auto& coordinate : robotData.coordinates)
	{
		cin >> coordinate;
	}

	for (auto& parameter : robotData.parameters)
	{
		cin >> parameter;
	}

	return cin;
}

}
