#include "Logger.h"


namespace logger
{

Logger::Logger(const std::string& file)
    : outFile(file, std::ios::app),
      _hasAnyErrors(outFile.is_open())
{
}

Logger::~Logger()
{
    outFile.close();
}

bool Logger::hasAnyErrors() const
{
    return _hasAnyErrors;
}
    
}
