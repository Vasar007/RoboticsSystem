#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>

#include "../Extension Functions/Utility.h"


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
     * \brief Set of type of streams.
     */
    enum class TypeStream
    {
        INPUT_STREAM,
        OUTPUT_STREAM
    };

    /**
     * \brief Input file stream.
     */
    std::ifstream inFile; // ORDER DEPENDENCY -> 1.

    /**
     * \brief Output file stream.
     */
    std::ofstream outFile; // ORDER DEPENDENCY -> 2.


private:
    /**
     * \brief Flag used to show if any errors in input stream occured.
     */
    bool _hasNotAnyInputErrors; // ORDER DEPENDENCY -> 3.    
    /**
     * \brief Flag used to show if any errors in ouput stream occured.
     */
    bool _hasNotAnyOutputErrors; // ORDER DEPENDENCY -> 4.


    /**
     * \brief               Restart current stream.
     * \tparam Stream       Type of stream to restart.
     * \param[out] stream   Stream to restart.
     */
    template <class Stream>
    void restart(Stream& stream);


public:
    /**
     * \brief                   Constructor.
     * \param[in] inputFile     File name for input. 
     * \param[in] outputFile    File name for output.
     */
    explicit Logger(const std::string& inputFile, const std::string& outputFile);

    /**
     * \brief Destructor.
     */
    ~Logger();

    /**
     * \brief       Write data in output stream.
     * \tparam T    Type of output data.
     * \param[in] t Data to write.
     */
    template <typename T>
    void write(const T& t);

    /**
     * \brief           Write (unlimited) data in output stream.
     * \tparam T        Type of output data.
     * \tparam Args     Variadic data types.
     * \param[in] t     Data to write.
     * \param[in] args  Data to write.
     */
    template <typename T, typename ...Args>
    void write(const T& t, const Args&... args);

    /**
     * \brief       Read data from input stream.
     * \tparam T    Type of input data.
     * \return      Readed data.
     */
    template <typename T>
    T read();

    /**
     * \brief   Show if any errors in input stream occurred.
     * \return  Flag that contains answer.
     */
    bool hasAnyInputErrors() const;

    /**
     * \brief   Show if any errors in output stream occurred.
     * \return  Flag that contains answer.
     */
    bool hasAnyOutputErrors() const;

    /**
     * \brief   Show if any errors in input and output streams occurred.
     * \return  Flag that contains answer.
     */
    bool hasAnyErrors() const;

    /**
     * \brief               Restart file stream.
     * \param[in] ioStream  Type of stream to restart.
     */
    void restartStream(const TypeStream ioStream);
};

#include "Logger.inl"

}

#endif // LOGGER_H
