#include <algorithm>
#include <sstream>
#include <vector>

#include "RobotData/RobotData.h"
#include "Utility/Utility.h"

#include "Parsing.h"


namespace utils
{

[[nodiscard]]
std::string parseFullData(const std::string& data, const int numberOfCoords,
                          const int excludeNumber) noexcept
{
    if (numberOfCoords <= excludeNumber || data.empty())
    {
        return { "" };
    }

    auto strStorage = utils::split<std::vector<std::string>>(data);

    strStorage.erase(std::remove(strStorage.begin(), strStorage.end(), ""), strStorage.end());

    if (strStorage.size() % numberOfCoords != 0u)
    {
        return { "" };
    }

    std::string result;
    int count = 0;
    for (const auto& str : strStorage)
    {
        if (!utils::isCorrectNumber(str))
        {
            result.clear();
            break;
        }

        bool flag = true;
        for (int i = 0; i < excludeNumber; ++i)
        {
            if (count % numberOfCoords == numberOfCoords - i - 1)
            {
                flag = false;
                break;
            }
        }
        if (flag)
        {
            result += str + " ";
        }

        ++count;
    }

    return result;
}

std::deque<vasily::RobotData> parseData(const std::string_view data) noexcept
{
    std::deque<vasily::RobotData> result;

    std::stringstream rawData(data.data());
    vasily::RobotData robotData;
    while (rawData >> robotData)
    {
        if (!rawData.fail())
        {
            result.emplace_back(robotData);
        }
    }
    return result;
}

std::pair<vasily::CoordinateSystem, bool> parseCoordinateSystem(
    const std::string_view data) noexcept
{
    const bool parsedResult = data.size() == 1u && utils::isCorrectNumber(data);

    if (!parsedResult)
    {
        return { vasily::CoordinateSystem::INVALID, parsedResult };
    }
    if (const int type = utils::stringToInt(data); 0 <= type && type <= 2)
    {
        return { static_cast<vasily::CoordinateSystem>(type), parsedResult };
    }

    return { vasily::CoordinateSystem::INVALID, parsedResult };
}

} // namespace utils
