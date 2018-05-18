#ifndef LOGGER_INL
#define LOGGER_INL


template <class Stream>
void Logger::restart(Stream& stream)
{
    if constexpr (std::is_same<Stream, std::ifstream>::value)
    {
        stream.clear();
        stream.seekg(0, std::ios::beg);
    }
    else if constexpr (std::is_same<Stream, std::ofstream>::value)
    {
        stream.clear();
        stream.seekp(0, std::ios::beg);
    }
    else
    {
        stream.clear();
    }
}

template <class T>
void Logger::writeImpl(const T& t)
{
    // It is necessary to flush buffer here.
    outFile << t << ' ' << std::flush;
    _hasNotAnyOutputErrors = !outFile.fail();
}

template <class T, class... Args>
void Logger::writeImpl(const T& t, const Args&... args)
{
    outFile << t << ' ';
    _hasNotAnyOutputErrors = !outFile.fail();

    writeImpl(args...);
}

template <class T>
void Logger::writeLineImpl(const T& t)
{
    // std::endl because it is necessary to flush buffer here.
    outFile << t << std::endl;
    _hasNotAnyOutputErrors = !outFile.fail();
}

template <class T, class... Args>
void Logger::writeLineImpl(const T& t, const Args&... args)
{
    outFile << t << ' ';
    _hasNotAnyOutputErrors = !outFile.fail();

    writeLineImpl(args...);
}

template <class... Args>
void Logger::write(const Args&... args)
{
    std::lock_guard lockGuard(_mutex);
    if (outFile)
    {
        writeImpl(args...);
    }
    else
    {
        repoortError("Data could not be written to file!");
    }
}

template <class... Args>
void Logger::writeLine(const Args&... args)
{
    std::lock_guard lockGuard(_mutex);
    if (outFile)
    {
        writeLineImpl(args...);
    }
    else
    {
        repoortError("Data could not be written to file!");
    }
}

template <class T>
T Logger::read()
{
    std::string readedData;
    if (inFile)
    {
        inFile >> readedData;
    }
    else
    {
        repoortError("File could not be opened!");
    }

    bool flag;
    T t = utils::fromString<T>(readedData, flag);

    _hasNotAnyInputErrors = !inFile.fail() && flag;

    return t;
}

template <class T>
T Logger::readLine()
{  
    std::string readedData;
    if (inFile)
    {
        std::getline(inFile, readedData);
    }
    else
    {
        repoortError("File could not be opened!");
    }

    bool flag;
    T t = utils::fromString<T>(readedData, flag);

    _hasNotAnyInputErrors = !inFile.fail() && flag;

    return t;
}

#endif // LOGGER_INL
