#include <random>
#include <chrono>
#include <ctime>
#include <cassert>

#include "Print/Print.h"
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
	
	auto RANDOM_ENGINE = createRandomEngine();

} // anonymous namespace

[[nodiscard]]
int randomInt(const int exclusiveMax) noexcept
{
	if (exclusiveMax <= 0)
	{
		return 0;
	}

	const std::uniform_int_distribution<> distr(0, exclusiveMax - 1);
	return distr(RANDOM_ENGINE);
}

bool isCorrectNumber(const std::string_view str, const int flag) noexcept
{
	char* ptr;

	const double out = strtod(str.data(), &ptr);

	if (flag == 1)
	{
		utils::println(std::cout, '\n', out);
	}

	return *ptr == 0;
}

[[nodiscard]]
int stringToInt(const std::string_view str) noexcept
{
	if (utils::isCorrectNumber(str))
	{
		return std::atoi(str.data());
	}

	return 0;
}

std::string getCurrentSystemTime() noexcept
{
	// Get current time.
	const auto timeNow = std::chrono::system_clock::now();

	// Convert time to ctime.
	std::time_t timeNowT = std::chrono::system_clock::to_time_t(timeNow);

	// Parse time in readable format.
	std::tm timeInfo;
	localtime_s(&timeInfo, &timeNowT);

	// The string result produced by asctime_s contains exactly 26 characters and
	// has the form => Wed Jan 02 02:03:55 1980\n\0.
	constexpr std::size_t SIZE = 26u;
	char timebuf[SIZE];
	const errno_t err = asctime_s(timebuf, SIZE, &timeInfo);

	if (err)
	{
		std::cout << "Error code: " << err << '\n';
		return { "" };
	}

	return timebuf;
}

bool almostEqual2Complement(float a, float b, const int maxUlps) noexcept
{
	// maxUlps must not be negative and not too large to NaN was not equal to any number.
	assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);

	int aInt = *reinterpret_cast<int*>(&a);
	// Remove sign in aInt, if you have to get the correct ordered sequence.
	if (aInt < 0)
	{
		// aInt &= 0x7fffffff;
		aInt = 0x80000000 - aInt;
	}

	// Similarly for bInt.
	int bInt = *reinterpret_cast<int*>(&b);
	if (bInt < 0)
	{
		// bInt &= 0x7fffffff;
		bInt = 0x80000000 - bInt;
	}

	const int intDiff = std::abs(aInt - bInt);

	return intDiff <= maxUlps;
}

} // namespace utils
