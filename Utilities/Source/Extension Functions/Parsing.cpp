#include <vector>
#include <algorithm>

#include "Utility.h"
#include "Parsing.h"


namespace utils
{

[[nodiscard]]
std::string parseFullData(const std::string& data) noexcept
{
	std::string result;
	std::vector<std::string> strStorage = utils::split<std::vector<std::string>>(data);

	strStorage.erase(std::remove(strStorage.begin(), strStorage.end(), ""), strStorage.end());

	constexpr std::size_t NUMBER_OF_COORDINATES_IN_ONE_STRUCTURE = 9u;

	if (strStorage.size() % NUMBER_OF_COORDINATES_IN_ONE_STRUCTURE != 0u)
	{
		return { "" };
	}

	std::size_t count = 0u;

	for (const auto& strDatum : strStorage)
	{
		if (!utils::isCorrectNumber(strDatum))
		{
			result.clear();
			break;
		}

		if (count % 9u != 8u && count % 9u != 7u)
		{
			result += strDatum + " ";
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
