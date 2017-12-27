#include <cassert>

#include "Logger.h"


namespace logger
{

Logger::Logger(const std::string& inputFile, const std::string& outputFile)
    : inFile(inputFile),
      outFile(outputFile, std::ios::app),
      _hasNotAnyInputErrors(inFile.is_open()),
      _hasNotAnyOutputErrors(outFile.is_open())
{
}

Logger::~Logger() noexcept
{
    inFile.close();
    outFile.close();
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
            restart(inFile);
            break;

        case TypeStream::OUTPUT_STREAM:
            restart(outFile);
            break;

        default:
            assert(false);
            break;
    }
}

} // namespace logger
