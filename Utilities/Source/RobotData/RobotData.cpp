#include <algorithm>
#include <iterator>
#include <sstream>

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
    double result = 0.0;

    for (std::size_t i = 0; i < NUMBER_OF_COORDINATES; ++i)
    {
        result += 1.0 * coordinates.at(i) * coordinates.at(i) / (TRANSFER_NUMBER * TRANSFER_NUMBER);
    }

    return std::sqrt(result);
}

std::istream& operator>>(std::istream& in, RobotData& robotData)
{
    for (auto& coordinate : robotData.coordinates)
    {
        in >> coordinate;
    }

    for (auto& parameter : robotData.parameters)
    {
        in >> parameter;
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const RobotData& robotData)
{
    std::copy(robotData.coordinates.begin(), robotData.coordinates.end(),
              std::ostream_iterator<int>(out, " "));

    std::copy(robotData.parameters.begin(), robotData.parameters.end(),
              std::ostream_iterator<int>(out, " "));

    return out;
}

std::wostream& operator<<(std::wostream& out, const RobotData& robotData)
{
    std::copy(robotData.coordinates.begin(), robotData.coordinates.end(),
              std::ostream_iterator<int, wchar_t>(out, L" "));

    std::copy(robotData.parameters.begin(), robotData.parameters.end(),
              std::ostream_iterator<int, wchar_t>(out, L" "));

    return out;
}

} // namespace vasily
