#include "Print.h"


namespace utils
{

void stdoutPrint(const std::string_view str) noexcept
{
	std::cout << str << ' ';
}


void stdoutPrintln(const std::string_view str) noexcept
{
	std::cout << str << '\n';
}

}
