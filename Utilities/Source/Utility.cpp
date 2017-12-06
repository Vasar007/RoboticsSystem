#include <random>
#include <chrono>
#include <thread>
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
		utils::println('\n', out);
	}

	return *ptr == 0;
}

int stringToInt(const std::string& str) noexcept
{
	if (utils::isCorrectNumber(str))
	{
		return std::atoi(str.c_str());
	}

	return 0;
}

}
