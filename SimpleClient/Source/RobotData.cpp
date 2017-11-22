#include <sstream>
#include <iostream>

#include "RobotData.h"

namespace vasily
{

RobotData::RobotData()
: mValues()
, mParameters()
{
}

std::string RobotData::toString() const
{
	std::stringstream stringStream;
	
	for (const auto& coordinate : mValues)
	{
		stringStream << coordinate << ' ';
	}

	for (const auto& parameter : mParameters)
	{
		stringStream << parameter << ' ';
	}

	return stringStream.str();
}

std::istream& operator>>(std::istream& cin, RobotData& robotData)
{
	for (auto& coordinate : robotData.mValues)
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
