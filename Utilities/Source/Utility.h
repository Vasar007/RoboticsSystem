#ifndef UTILITY_H
#define UTILITY_H
#pragma once

#include <string>
#include <sstream>
#include <memory>
#include <utility>


// Forward declaration.
class Client;
class ServerImitator;

/**
 * \brief Unique namespace for utilities function.
 */
namespace utils
{

/**
 * \brief		Function checks whether the templates are same datatype at compile time.
 * \tparam T	Datatype to check.
 * \return		True — current data types are same, false — otherwise.
 */
template <class T, class U>
constexpr bool		isSame() { return std::is_same<T, U>::value; }


/**
 * \brief				One of the common implementation of the splitting strings.
 * \tparam Container	Type of the contatiner for output.
 * \param str			String to split.
 * \param cont			Container for output.
 * \param delims		Delimiter characters.
 */
template <class Container>
void				split(const std::string& str, Container& cont, 
							const std::string& delims = " ") noexcept;


/**
 * \brief		Since std::to_string doesn't work on MinGW we have to implement our own
 *				to support all platforms.
 * \tparam T	Primitive datatype (doesn't work with user's objects).
 * \param value Value which will be converted to string.
 * \return		String created from an object.
 */
template <typename T>
std::string			toString(const T& value) noexcept;


/**
 * \brief			Correct copying unique pointers.
 * \tparam T		Datatype which contains unique pointer.
 * \param source	The primary source for copying.
 * \return			Copied unique pointer.
 */
template <class T>
std::unique_ptr<T>	copyUnique(const std::unique_ptr<T>& source) noexcept;


/**
 * \brief			Own implementation of swapping.
 * \tparam T		Datatype for swapping.
 * \param first		First object for swapping.
 * \param second	Second object for swapping.
 */
template <class T>
void				swap(T& first, T& second) noexcept;


/**
 * \brief		Fuction checks if input string is correct number at all (heximal, decimal, binary).
 * \param str	String to check.
 * \param flag	Additional flag to perfom some actions.
 * \return		True if string is number, false otherwise.
 */
bool				isCorrectNumber(const std::string& str, int flag = 0) noexcept;

#include "Utility.inl"
}

#endif // UTILITY_H
