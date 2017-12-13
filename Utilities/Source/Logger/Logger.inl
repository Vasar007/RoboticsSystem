#ifndef LOGGER_INL
#define LOGGER_INL


template <class Stream>
void Logger::restart(Stream& stream)
{
    if constexpr (utils::isSame<Stream, std::ifstream>())
    {
        stream.clear();
        stream.seekg(0, std::ios::beg);
    }
    else if constexpr (utils::isSame<Stream, std::ofstream>())
    {
        stream.clear();
        stream.seekp(0, std::ios::beg);
    }
    else
    {
        stream.clear();
    }
}

template <typename T>
void Logger::write(const T& t)
{
    // std::endl because it is necessary to flush buffer here.
    outFile << t << std::endl;
    _hasNotAnyOutputErrors = outFile.good();
}

template <typename T, typename ...Args>
void Logger::write(const T& t, const Args&... args)
{
    outFile << t << ' ';
    _hasNotAnyOutputErrors = outFile.good();

    write(args...);
}

template <typename T>
T Logger::read()
{  
    std::string readedData;
    if (!inFile.eof())
    {
        std::getline(inFile, readedData);
    }

    bool flag;
    T t = utils::fromString<T>(readedData, flag);

    _hasNotAnyInputErrors = inFile.good() && flag;

    return t;
}


#endif // LOGGER_INL
