#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <sstream>
#include <memory>
#include <utility>
#include <string_view>


// Forward declaration.
class Client;
class ServerImitator;

/**
 * \brief Unique namespace for utilities functions.
 */
namespace utils
{

/**
 * \brief		Check whether the templates are same datatype at compile time.
 * \tparam T	Datatype to check.
 * \return		True — current data types are same, false — otherwise.
 */
template <class T, class U>
constexpr bool		isSame() noexcept;


/**
 * \brief			Since std::to_string doesn't work on MinGW we have to implement our own
 *					to support all platforms.
 * \details			Function doesn't work with user's objects.
 * \tparam T		Primitive datatype.
 * \param[in] value Value which will be converted to string.
 * \return			String created from an object.
 */
template <typename T>
std::string			toString(const T& value) noexcept;


/**
 * \brief			Try convert string to any data.
 * \details			Type of data which used to convert must have default constructor.
 * \tparam T		Type of data in which we try convert string.
 * \param[in] str	String to convert.
 * \param[out] ok	Flag used to define operation success.
 * \return			Converted filled data or empty data.
 */
template <typename T>
T					fromString(const std::string& str, bool& ok) noexcept;


/**
 * \brief			Primitive own template deduction for function with trivial case.
 * \param[in] str	String to convert.
 * \param[out] ok	Flag used to define operation success.
 * \return			Exact same string which forwards into function.
 */
template <>
inline std::string	fromString(const std::string& str, bool& ok) noexcept;


/**
 * \brief				Correct copying unique pointers.
 * \tparam T			Datatype which contains unique pointer.
 * \param[in] source	The primary source for copying.
 * \return				Copied unique pointer.
 */
template <typename T>
std::unique_ptr<T>	copyUnique(const std::unique_ptr<T>& source) noexcept;


/**
 * \brief				Own implementation of swapping.
 * \tparam T			Datatype for swapping.
 * \param[out] first	First object for swapping.
 * \param[out] second	Second object for swapping.
 */
template <class T>
void				swap(T& first, T& second) noexcept;


/**
 * \brief			Check if input string is correct number at all 
 *					(heximal, decimal, binary).
 * \param[in] str	String to check.
 * \param[in] flag	Additional flag to perfom some actions.
 * \return			True if string is number, false otherwise.
 */
bool				isCorrectNumber(const std::string& str, const int flag = 0) noexcept;


/**
 * \brief					Random number generation.
 * \param[in] exclusiveMax	Exclusive maximum of the interval.
 * \return					Random generated integer in interval.
 */
[[nodiscard]]
int					randomInt(const int exclusiveMax) noexcept;


/**
 * \brief			Parse string to integer.
 * \param[in] str	String for parsing to integer.
 * \return			Parsed integer from string.
 */
[[nodiscard]]
int					stringToInt(const std::string& str) noexcept;


/**
 * \brief           Get current system time.
 * \param[in] flag  Additional flag to perfom some actions.
 * \return          String that cintains current readable time.
 */
std::string         getCurrentSystemTime(const int flag = 0);


#include "Utility.inl"

}

#endif // UTILITY_H
