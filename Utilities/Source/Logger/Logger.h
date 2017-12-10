#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>


/**
 * \brief Additional namespace to work with logging.
 */
namespace logger
{
    
/**
 * \brief Class used to write data to output file.
 */
class Logger
{
public:
    /**
     * \brief Output file stream.
     */
    std::ofstream outFile; // ORDER DEPENDENCY -> 1.


private:
    /**
     * \brief Flag used to show if any errors in ouput stream occured.
     */
    bool _hasAnyErrors; // ORDER DEPENDENCY -> 2.


public:
    /**
     * \brief           Constructor.
     * \param[in] file  File name for output.
     */
    explicit Logger(const std::string& file);

    /**
     * \brief Destructor.
     */
    ~Logger();

    /**
     * \brief       Function writes data in output stream.
     * \tparam T    Type of output data.
     * \param[in] t Data to write.
     */
    template <typename T>
    void write(const T& t);

    /**
     * \brief           Function writes (unlimited) data in output stream.
     * \tparam T        Type of output data.
     * \tparam Args     Variadic data types.
     * \param[in] t     Data to write.
     * \param[in] args  Data to write.
     */
    template <typename T, typename ...Args>
    void write(const T& t, const Args&... args);

    /**
     * \brief   Function shows if any errors in output stream occured.
     * \return  Flag that contains answer.
     */
    bool hasAnyErrors() const;
};

#include "Logger.inl"

}

#endif // LOGGER_H
