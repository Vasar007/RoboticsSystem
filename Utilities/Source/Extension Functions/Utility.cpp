#include <random>
#include <chrono>
#include <ctime>

#include "Print.h"
#include "Utility.h"


namespace utils
{

namespace
{

	std::default_random_engine createRandomEngine()
	{
		const auto seed = static_cast<unsigned long>(std::time(nullptr));
		return std::default_random_engine(seed);
	}
	
	auto randomEngine = createRandomEngine();

}

[[nodiscard]]
int randomInt(const int exclusiveMax) noexcept
{
	if (exclusiveMax <= 0)
	{
		return 0;
	}

	const std::uniform_int_distribution<> distr(0, exclusiveMax - 1);
	return distr(randomEngine);
}

bool isCorrectNumber(const std::string& str, const int flag) noexcept
{
	char* ptr;

	const double out = strtod(str.c_str(), &ptr);

	if (flag == 1)
	{
		utils::println(std::cout, '\n', out);
	}

	return *ptr == 0;
}

[[nodiscard]]
int stringToInt(const std::string& str) noexcept
{
	if (utils::isCorrectNumber(str))
	{
		return std::stoi(str.c_str());
	}

	return 0;
}

std::string getCurrentSystemTime(const int flag)
{
	// Get current time.
	const auto timeNow = std::chrono::system_clock::now();

	// Convert time to ctime.
	std::time_t timeNowT = std::chrono::system_clock::to_time_t(timeNow);

	// Parse time in readable format.
	tm timeInfo;
	localtime_s(&timeInfo, &timeNowT);
	char timebuf[26];
	const errno_t err = asctime_s(timebuf, 26, &timeInfo);

	if (flag == 1)
	{
		utils::println(std::cout, '\n', err);
	}

	return timebuf;
}

}
