#ifndef PARSING_H
#define PARSING_H

#include <string>


namespace utils
{

/**
 * \brief			Parse input data and skips every 8 and 9 number.
 * \details         Data contain one or more RobotData structure transfered into string.
 * \param[in] data	Data for parsing.
 * \return			Deserialized data if parsed successful, empty string otherwise.
 */
[[nodiscard]]
std::string parseFullData(const std::string& data) noexcept;


/**
 * \brief           Parse input data.
 * \param[in] data  Data for parsing.
 * \return          Deserialized data if parsed successful, empty string otherwise.
 */
[[nodiscard]]
std::string parseData(const std::string& data) noexcept;


/**
 * \brief					Split string by delimeter characters but doesn't include them.
 * \tparam Container		Type of the contatiner for output.
 * \param[in] str			String to split.
 * \param[in] delims		Delimiter characters (not STRINGS!).
 */
template <class Container>
Container	split(const std::string& str, const std::string_view delims = " ") noexcept;


/**
 * \brief					Split string by delimeter string and includes it.
 * \tparam Container		Type of the contatiner for output.
 * \param[in] str			String to split.
 * \param[in] delim			Delimiter string (not CHARACTERS!).
 */
template <class Container>
Container	fsplit(const std::string& str, const std::string_view delim = " ") noexcept;

#include "Parsing.inl"

}

#endif // PARSING_H
