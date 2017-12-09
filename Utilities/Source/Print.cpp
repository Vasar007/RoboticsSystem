#include "Print.h"


namespace utils
{

void print(const std::string_view str) noexcept
{
	std::cout << str << '\n';
}


void println(const std::string_view str) noexcept
{
	std::cout << str << '\n';
}

}
