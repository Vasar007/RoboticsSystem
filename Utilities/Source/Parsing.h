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
std::string parseData(const std::string& data) noexcept;

}

#endif // PARSING_H
