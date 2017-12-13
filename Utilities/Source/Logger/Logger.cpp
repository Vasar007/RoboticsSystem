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

Logger::~Logger()
{
    inFile.close();
    outFile.close();
}

bool Logger::hasAnyInputErrors() const
{
    return !_hasNotAnyInputErrors;
}

bool Logger::hasAnyOutputErrors() const
{
    return !_hasNotAnyOutputErrors;
}

bool Logger::hasAnyErrors() const
{
    return !_hasNotAnyInputErrors && !_hasNotAnyOutputErrors;
}

void Logger::restartStream(const TypeStream ioStream)
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

}
