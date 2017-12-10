#ifndef PRINT_H
#define PRINT_H

#include <iostream>
#include <string_view>
#include <type_traits>
#include <typeinfo>

#ifndef _MSC_VER
	#include <cxxabi.h>
#endif


namespace utils
{

/**
 * \brief			Function prints string or space by default to outstream.
 * \param[in] str   String to print.
 */
void stdoutPrint(const std::string_view str = " ") noexcept;


/**
 * \brief		        Fuction prints datatype to outstream.
 * \tparam OutputStream	Type of output stream.
 * \tparam T	        Datatype to print (need to be overloaded operator '<<').
 * \param[out] out      Output stream to write.
 * \param[in] t         Data to print.
 */
template <class OutputStream, typename T>
void print(OutputStream& out, const T& t) noexcept;


/**
 * \brief			    Fuction prints datatypes (unlimited) to outstream.
 * \tparam OutputStream	Type of output stream.
 * \tparam T		    Datatype to print (need to be overloaded operator '<<').
 * \tparam Args		    Variadic datatypes (need to be overloaded operator '<<').
 * \param[out] out      Output stream to write.
 * \param[in] t		    Data to print.
 * \param[in] args	    Data to print.
 */
template <class OutputStream, typename T, typename... Args>
void print(OutputStream& out, const T& t, const Args&... args) noexcept;


/**
 * \brief			Function prints string or endline by default to outstream.
 * \param[in] str	String to print.
 */
void stdoutPrintln(const std::string_view str = "") noexcept;


/**
 * \brief		        Fuction prints datatype to outstream.
 * \tparam OutputStream	Typpe of output stream.
 * \tparam T	        Datatype to print (need to be overloaded operator '<<').
 * \param[out] out      Output stream to write.
 * \param[in] t         Data to print.
 */
template <class OutputStream, typename T>
void println(OutputStream& out, const T& t) noexcept;


/**
 * \brief			    Fuction prints datatypes (unlimited) to outstream.
 * \tparam OutputStream	Typpe of output stream.
 * \tparam T		    Datatype to print (need to be overloaded operator '<<').
 * \tparam Args		    Variadic datatypes (need to be overloaded operator '<<').
 * \param[out] out      Output stream to write.
 * \param[in] t		    Data to print.
 * \param[in] args	    Data to print.
 */
template <class OutputStream, typename T, typename... Args>
void println(OutputStream& out, const T& t, const Args&... args) noexcept;


/**
 * \brief		Function knows about type name of variable.
 * \tparam T	Datatype to process (use 'decltype()' to send data).
 * \return		String which contains type name of variable.
 */
template <class T>
constexpr std::string_view typeName() noexcept;

#include "Print.inl"

}

#endif // PRINT_H
