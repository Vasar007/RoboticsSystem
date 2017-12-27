#include <vector>
#include <algorithm>

#include "../Utility/Utility.h"
#include "../Print/Print.h"
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
	std::vector<std::string> strStorage = utils::split<std::vector<std::string>>(data);

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

// TODO: finish this function with Danila.
std::string parseData(const std::string& data) noexcept
{
	std::vector<std::string> strStorage = utils::split<std::vector<std::string>>(data);

	int count = 0;
	for (const auto& str : strStorage)
	{
		if (utils::isCorrectNumber(str))
		{
			++count;
		}
	}

	return "";
}

} // namespace utils
