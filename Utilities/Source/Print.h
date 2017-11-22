#ifndef PRINT_H
#define PRINT_H
#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <memory>
#include <cstdlib>

#ifndef _MSC_VER
	#include <cxxabi.h>
#endif


namespace utils
{

/**
 * \brief			Function prints string or space by default to outstream.
 * \param[in] str	String to print.
 */
void print(std::string_view str = " ");


/**
 * \brief		Fuction prints datatype to outstream.
 * \tparam T	Datatype to print (need to be overloaded operator '<<').
 * \param[in] t Data to print.
 */
template <typename T>
void print(const T& t);


/**
 * \brief			Fuction prints datatypes (unlimited) to outstream.
 * \tparam T		Datatype to print (need to be overloaded operator '<<').
 * \tparam Args		Variadic datatypes (need to be overloaded operator '<<').
 * \param[in] t		Data to print.
 * \param[in] args	Data to print.
 */
template<typename T, typename... Args>
void print(const T& t, const Args&... args);


/**
 * \brief			Function prints string or endline by default to outstream.
 * \param[in] str	String to print.
 */
void println(std::string_view str = "");


/**
 * \brief		Fuction prints datatype to outstream.
 * \tparam T	Datatype to print (need to be overloaded operator '<<').
 * \param[in] t Data to print.
 */
template <typename T>
void println(const T& t);


/**
 * \brief			Fuction prints datatypes (unlimited) to outstream.
 * \tparam T		Datatype to print (need to be overloaded operator '<<').
 * \tparam Args		Variadic datatypes (need to be overloaded operator '<<').
 * \param[in] t		Data to print.
 * \param[in] args	Data to print.
 */
template <typename T, typename... Args>
void println(const T& t, const Args&... args);


/**
 * \brief		Function knows about type name of variable.
 * \tparam T	Datatype to process (use 'decltype()' to send data).
 * \return		String which contains type name of variable.
 */
template <class T>
constexpr std::string_view typeName();

#include "Print.inl"
}

#endif // PRINT_H
