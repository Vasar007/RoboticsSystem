#ifndef LOGGER_INL
#define LOGGER_INL


template <typename T>
void Logger::write(const T& t)
{
    // std::endl because it is necessary to flush buffer here.
    outFile << t << std::endl;
    _hasAnyErrors = outFile.good();
}

template <typename T, typename ...Args>
void Logger::write(const T& t, const Args&... args)
{
    outFile << t << ' ';
    _hasAnyErrors = outFile.good();

    write(args...);
}

#endif // LOGGER_INL
