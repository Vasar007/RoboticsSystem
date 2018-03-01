#ifndef PARSING_H
#define PARSING_H

#include <deque>
#include <functional>
#include <string>


// Forward declaration.
namespace vasily
{

struct RobotData;

enum class CoordinateSystem;

}

namespace utils
{

/**
 * \brief			            Parse input data and skips every 8 and 9 number.
 * \details                     Data contain one or more RobotData structure transfered into string.
 * \param[in] data	            Data for parsing.
 * \param[in] numberOfCoords	Number of coordinates in one structure.
 * \param[in] excludeNumber	    Number of exclude coordintes.
 * \return			            Deserialized data if parsed successful, empty container otherwise.
 */
[[nodiscard]]
std::string parseFullData(const std::string& data, const int numberOfCoords = 9,
                          const int excludeNumber = 2) noexcept;


/**
 * \brief           Parse input data to queue as messages.
 * \param[in] data  Data for parsing.
 * \return          Deserialized data if parsed successful, empty container otherwise.
 */
[[nodiscard]]
std::deque<vasily::RobotData> parseData(const std::string_view data) noexcept;


/**
 * \brief           Check if input data are coordinate type or not.
 * \param[in] data  Data for parsin.
 * \return          True if input data contains only one digit, false otherwise.
 */
std::pair<vasily::CoordinateSystem, bool> parseCoordinateSystem(
    const std::string_view data) noexcept;


/**
 * \brief					Split string by delimeter characters and don't include them.
 * \tparam Container		Type of the contatiner for output.
 * \param[in] str			String to split.
 * \param[in] delims		Delimiter characters (not STRINGS!).
 */
template <class Container>
Container	split(const std::string& str, const std::string_view delims = " ") noexcept;


/**
 * \brief					Split string by delimeter string and don't includes it.
 * \tparam Container		Type of the contatiner for output.
 * \param[in] str			String to split.
 * \param[in] delim			Delimiter string (not CHARACTERS!).
 */
template <class Container>
Container	fsplit(const std::string& str, const std::string_view delim = " ") noexcept;

#include "Parsing.inl"

} // namespace utils

#endif // PARSING_H
