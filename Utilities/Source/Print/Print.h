#ifndef PRINT_H
#define PRINT_H

#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <string>
#include <string_view>

#ifndef _MSC_VER
	#include <cxxabi.h>
#endif

#include "Print.hpp"


namespace utils
{

/**
 * \brief           Print string to standart outstream.
 * \param[in] str   String to print.
 */
void stdOutput(const std::string_view str) noexcept;


/**
 * \brief           Fast print string to outstream.
 * \param[in] str   String to print.
 */
void fastOutput(const std::string& str) noexcept;


/**
 * \brief		Define the type name of variable.
 * \tparam T	Datatype to process (use 'decltype()' to send data).
 * \return		String which contains type name of variable.
 */
template <class T>
constexpr std::string_view typeName() noexcept;

#include "Print.inl"

} // namespace utils

#endif // PRINT_H
