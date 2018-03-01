#include <cassert>
#include <cstring>
#include <iostream>
#include <iterator>

#include "Logger.h"


namespace logger
{

Logger::Logger(const std::string& inputFile, const std::string& outputFile)
    : inFile(inputFile),
      outFile(outputFile, std::ios::app),
      _hasNotAnyInputErrors(inFile.is_open()),
      _hasNotAnyOutputErrors(outFile.is_open())
{
    if (hasAnyErrors())
    {
        repoortError("Problem with initialization streams!");
    }
}

Logger::~Logger() noexcept
{
    inFile.close();
    outFile.close();
}

void Logger::repoortError(const std::string_view message) const noexcept
{
    std::cerr << message << '\n'; // Report error.
    char sysMsg[64];
    std::cerr << "Error code: " << strerror_s(sysMsg, errno); // Get some info as to why.
}

bool Logger::hasAnyInputErrors() const noexcept
{
    return !_hasNotAnyInputErrors;
}

bool Logger::hasAnyOutputErrors() const noexcept
{
    return !_hasNotAnyOutputErrors;
}

bool Logger::hasAnyErrors() const noexcept
{
    return !_hasNotAnyInputErrors && !_hasNotAnyOutputErrors;
}

void Logger::restartStream(const TypeStream ioStream) noexcept
{
    switch (ioStream)
    {
        case TypeStream::INPUT_STREAM:
            _hasNotAnyInputErrors = true;
            restart(inFile);
            break;

        case TypeStream::OUTPUT_STREAM:
            _hasNotAnyOutputErrors = true;
            restart(outFile);
            break;

        default:
            assert(false);
            break;
    }
}

std::size_t Logger::countLinesInInputFile() noexcept
{
    // New lines will be skipped unless we stop it from happening: 
    inFile.unsetf(std::ios_base::skipws);

    // Count the newlines with an algorithm specialized for counting:
    const std::size_t lineCount = std::count(std::istream_iterator<char>(inFile),
                                             std::istream_iterator<char>(), '\n') + 1;

    inFile.setf(std::ios_base::skipws);
    restartStream(TypeStream::INPUT_STREAM);

    return lineCount;
}
} // namespace logger
