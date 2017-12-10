#include <sstream>
#include <iostream>
#include <cassert>
#include <iterator>

#include "Utilities.h"
#include "RobotData.h"


namespace vasily
{

std::string RobotData::toString() const
{
	std::stringstream stringStream;

	std::copy(mCoordinates.begin(), mCoordinates.end(), 
			  std::ostream_iterator<int>(stringStream, " "));

	std::copy(mParameters.begin(), mParameters.end(),
			  std::ostream_iterator<int>(stringStream, " "));
	
	return stringStream.str();
}

double RobotData::length() const
{
	long long result = 0;
	for (const auto& coordinate : mCoordinates)
	{
		result += coordinate * coordinate;
	}

	return std::sqrt(result);
}

bool RobotData::isEqual(const RobotData& other) const
{
	return this->mCoordinates == other.mCoordinates && this->mParameters == other.mParameters;
}

bool operator ==(const RobotData& lhs, const RobotData& rhs)
{
	return lhs.mCoordinates == rhs.mCoordinates;
}

bool operator !=(const RobotData& lhs, const RobotData& rhs)
{
	return lhs.mCoordinates != rhs.mCoordinates;
}

bool operator <(const RobotData& lhs, const RobotData& rhs)
{
	return lhs.mCoordinates < rhs.mCoordinates;
}

bool operator >(const RobotData& lhs, const RobotData& rhs)
{
	return lhs.mCoordinates > rhs.mCoordinates;
}

RobotData operator -(const RobotData& lhs, const RobotData& rhs)
{
	RobotData result;
	for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
	{
		result.mCoordinates.at(i) = lhs.mCoordinates.at(i) - rhs.mCoordinates.at(i);
	}

	return result;
}

RobotData operator +(const RobotData& lhs, const RobotData& rhs)
{
	RobotData result;
	for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
	{
		result.mCoordinates.at(i) = lhs.mCoordinates.at(i) + rhs.mCoordinates.at(i);
	}

	return result;
}

RobotData& operator +=(RobotData& lhs, const RobotData& rhs)
{
	for (std::size_t i = 0u; i < RobotData::NUMBER_OF_COORDINATES; ++i)
	{
		lhs.mCoordinates.at(i) += rhs.mCoordinates.at(i);
	}

	return lhs;
}

RobotData operator /(const RobotData& lhs, const int& rhs)
{
	assert(rhs != 0);

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

RobotData& operator /=(RobotData& lhs, const int& rhs)
{
	assert(rhs != 0);

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

std::istream& operator >>(std::istream& cin, RobotData& robotData)
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

}
