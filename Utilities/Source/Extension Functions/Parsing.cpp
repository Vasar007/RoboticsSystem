#include <vector>
#include <algorithm>

#include "Utility.h"
#include "Print.h"
#include "Parsing.h"


namespace utils
{

[[nodiscard]]
std::string parseFullData(const std::string& data, const std::size_t numberOfCoords,
						  const std::size_t excludeNumber) noexcept
{
	if (numberOfCoords <= excludeNumber || data.empty())
	{
		return { "" };
	}

	std::string result;
	std::vector<std::string> strStorage = utils::split<std::vector<std::string>>(data);

	strStorage.erase(std::remove(strStorage.begin(), strStorage.end(), ""), strStorage.end());

	if (strStorage.size() % numberOfCoords != 0u)
	{
		return { "" };
	}

	std::size_t count = 0u;
	for (const auto& str : strStorage)
	{
		if (!utils::isCorrectNumber(str))
		{
			result.clear();
			break;
		}

		bool flag = true;
		for (std::size_t i = 0u; i < excludeNumber; ++i)
		{
			if (count % numberOfCoords == numberOfCoords - i - 1u)
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

// TODO: finish this function with Danila.
std::string parseData(const std::string& data) noexcept
{
	std::vector<std::string> strStorage = utils::split<std::vector<std::string>>(data);

	std::size_t count = 0u;
	for (const auto& str : strStorage)
	{
		if (utils::isCorrectNumber(str))
		{
			++count;
		}
	}

	return "";
}

}
