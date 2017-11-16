#include <iostream>

#include "Utility.h"


namespace utils
{

bool isCorrectNumber(const std::string& str, int flag) noexcept
{
	char* ptr;
	
	double out = strtod(str.c_str(), &ptr);

	if (flag == 1)
	{
		std::cout << '\n' <<  out << '\n';
	}

	return *ptr == 0;
}

}
