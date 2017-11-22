#ifndef PRINT_INL
#define PRINT_INL
#pragma once

template <typename T>
void print(const T& t)
{
	std::cout << t << ' ';
}

template <typename T, typename... Args>
void print(const T& t, const Args&... args)
{
	std::cout << t << ' ';
	print(args...);
}

template <typename T>
void println(const T& t)
{
	std::cout << t << '\n';
}

template <typename T, typename... Args>
void println(const T& t, const Args&... args)
{
	std::cout << t << ' ';
	println(args...);
}

template <class T>
constexpr std::string_view typeName()
{
#ifdef __clang__
	std::string_view p = __PRETTY_FUNCTION__;
	return std::string_view(p.data() + 34, p.size() - 34 - 1);
#elif defined(__GNUC__)
	string_view p = __PRETTY_FUNCTION__;
	#if __cplusplus < 201402
		return std::string_view(p.data() + 36, p.size() - 36 - 1);
	#else
		return std::string_view(p.data() + 49, p.find(';', 49) - 49);
	#endif
#elif defined(_MSC_VER)
	std::string_view p = __FUNCSIG__;
	return std::string_view(p.data() + 83, p.size() - 84 - 6);
#endif
}

#endif // PRINT_INL
