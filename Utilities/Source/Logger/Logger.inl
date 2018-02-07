#ifndef LOGGER_INL
#define LOGGER_INL


template <class Stream>
void Logger::restart(Stream& stream) noexcept
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

template <typename T>
void Logger::writeImpl(const T& t) noexcept
{
    // It is necessary to flush buffer here.
    outFile << t << ' ' << std::flush;
    _hasNotAnyOutputErrors = !outFile.fail();
}

template <typename T, typename ...Args>
void Logger::writeImpl(const T& t, const Args&... args) noexcept
{
    outFile << t << ' ';
    _hasNotAnyOutputErrors = !outFile.fail();

    writeImpl(args...);
}

template <typename T>
void Logger::writeLineImpl(const T& t) noexcept
{
    // std::endl because it is necessary to flush buffer here.
    outFile << t << std::endl;
    _hasNotAnyOutputErrors = !outFile.fail();
}

template <typename T, typename ...Args>
void Logger::writeLineImpl(const T& t, const Args&... args) noexcept
{
    outFile << t << ' ';
    _hasNotAnyOutputErrors = !outFile.fail();

    writeLineImpl(args...);
}

template <typename ...Args>
void Logger::write(const Args&... args) noexcept
{
    std::lock_guard<std::mutex> lockGuard{ _mutex };
    if (outFile)
    {
        writeImpl(args...);
    }
    else
    {
        repoortError("Data could not be written to file!");
    }
}

template <typename ...Args>
void Logger::writeLine(const Args&... args) noexcept
{
    std::lock_guard<std::mutex> lockGuard{ _mutex };
    if (outFile)
    {
        writeLineImpl(args...);
    }
    else
    {
        repoortError("Data could not be written to file!");
    }
}

template <typename T>
T Logger::read() noexcept
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

template <typename T>
T Logger::readLine() noexcept
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
