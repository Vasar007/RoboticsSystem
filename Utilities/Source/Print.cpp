#include "Print.h"


namespace utils
{

void print(std::string_view str)
{
	std::cout << str << '\n';
}


void println(std::string_view str)
{
	std::cout << str << '\n';
}

}
