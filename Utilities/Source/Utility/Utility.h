#ifndef UTILITY_H
#define UTILITY_H

#include <cassert>
#include <numeric>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>


#define DOUBLE_STATIC_CAST(Type, valueToCast) static_cast<Type>(static_cast<void*>(&valueToCast))

// Forward declaration.
class Client;
class ServerImitator;

/**
 * \brief Unique namespace for utilities functions.
 */
namespace utils
{

/**
 * \brief			Since std::to_string doesn't work on MinGW we have to implement our own
 *					to support all platforms.
 * \details			Function doesn't work with user's objects.
 * \tparam T		Primitive datatype.
 * \param[in] value Value which will be converted to string.
 * \return			String created from an object.
 */
template <class T>
std::string			toString(const T& value);


/**
 * \brief			Try convert string to any data.
 * \details			Type of data which used to convert must have default constructor.
 * \tparam T		Type of data in which we try convert string.
 * \param[in] str	String to convert.
 * \param[out] ok	Flag used to define operation success.
 * \return			Converted filled data or empty data.
 */
template <class T>
T	                fromString(const std::string& str, bool& ok);


/**
 * \brief		  Primitive own template deduction for function with trivial case.
 * \param[in] str String to convert.
 * \param[out] ok Flag used to define operation success.
 * \return		  Exact same string which forwards into function.
 */
template <>
inline std::string fromString(const std::string& str, bool& ok);


/**
 * \brief			 Correct copying unique pointers.
 * \tparam T		 Datatype which contains unique pointer.
 * \param[in] source The primary source for copying.
 * \return			 Copied unique pointer.
 */
template <class T>
std::unique_ptr<T>  copyUnique(const std::unique_ptr<T>& source);


/**
 * \brief			  Own implementation of swapping.
 * \tparam T		  Datatype for swapping.
 * \param[out] first  First object for swapping.
 * \param[out] second Second object for swapping.
 */
template <class T>
void               swap(T& first, T& second) noexcept(std::is_nothrow_move_constructible_v<T>
                                                      && std::is_nothrow_move_assignable_v<T>);


/**
 * \brief          Check if input string is correct number at all 
 *                 (heximal, decimal, binary).
 * \param[in] str  String to check.
 * \param[in] flag Additional flag to perfom some actions.
 * \return         True if string is number, false otherwise.
 */
bool               isCorrectNumber(const std::string_view str, const int flag = 0);


/**
 * \brief					  Random number generation.
 * \details                   Don't use it in multithread contexts.
 * \param[in] exclusiveBorder Exclusive border of the interval.
 * \return					  Random generated integer in interval.
 */
[[nodiscard]]
int                 randomInt(const int exclusiveBorder);


/**
 * \brief		  Parse string to integer.
 * \param[in] str String for parsing to integer.
 * \return		  Parsed integer from string or zero if input data is not integral.
 */
[[nodiscard]]
int                 stringToInt(const std::string_view str);


/**
 * \brief  Get current system time.
 * \return String that contains current readable time.
 */
std::string         getCurrentSystemTime() noexcept;


/**
 * \brief             Calculate distance as square root from scalar product of the vector
 *                    difference.
 * \tparam InputIt1   First iterator type.
 * \tparam InputIt2   Second iterator type.
 * \tparam T          Type of returned value.
 * \param[in] first1  The beginning of range of elements.
 * \param[in] last1   The ending of range of elements.
 * \param[in] first2  The beginning of the second range of elements.
 * \param[in] value   Initial value of the sum of the products.
 * \param[in] divisor Value used to divide initial product.
 * \return            Distance between two points.
 */
template <class InputIt1, class InputIt2, class T>
[[nodiscard]]
T                   distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, T value,
                             const T& divisor);

/**
 * \brief         Compare two not integral numbers.
 * \param[in] a   First number to compare.
 * \param[in] b   Second number to compare.
 * \param maxUlps The number of binary bits (starting with youngest) to compare the numbers
 *                allowed to miss.
 * \return        True if numbers almost equal, false — otherwise.
 */
[[nodiscard]]
bool                almostEqual2Complement(float a, float b, const int maxUlps);


#include "Utility.inl"

} // namespace utils

#endif // UTILITY_H
