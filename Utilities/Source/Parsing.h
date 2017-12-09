#ifndef PARSING_H
#define PARSING_H

#include <string>


namespace utils
{

/**
 * \brief			Function parses input data.
 * \param[in] data	Data for parsing.
 * \return			Deserialized data if parsed successful, empty string otherwise.
 */
[[nodiscard]]
std::string parseData(const std::string& data) noexcept;

/**
 * \brief					Function splits string by delimeter characters but doesn't include them.
 * \tparam Container		Type of the contatiner for output.
 * \param[in] str			String to split.
 * \param[out] cont			Container for output.
 * \param[in] delims		Delimiter characters (not STRINGS!).
 */
template <class Container>
void		split(const std::string& str, Container& cont,
				  const std::string_view delims = " ") noexcept;

/**
 * \brief					Function splits string by delimeter string and includes them.
 * \tparam Container		Type of the contatiner for output.
 * \param[in] str			String to split.
 * \param[out] cont			Container for output.
 * \param[in] delim			Delimiter string (not CHARACTERS!).
 */
template <class Container>
void		fsplit(const std::string& str, Container& cont,
				   const std::string_view delim = " ") noexcept;

#include "Parsing.inl"

}

#endif // PARSING_H
