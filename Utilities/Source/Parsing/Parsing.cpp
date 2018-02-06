#include <vector>
#include <algorithm>
#include <sstream>

#include "../Utility/Utility.h"
#include "../Print/Print.h"
#include "../RobotData/RobotData.h"
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

	std::string result;
	auto strStorage = utils::split<std::vector<std::string>>(data);

	strStorage.erase(std::remove(strStorage.begin(), strStorage.end(), ""), strStorage.end());

	if (strStorage.size() % numberOfCoords != 0u)
	{
		return { "" };
	}

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
			result.emplace_back(std::move(robotData));
		}
	}
	return result;
}

bool parseCoordinateType(const std::string_view data) noexcept
{
	return data.size() == 1u && isCorrectNumber(data.data());
}

} // namespace utils
