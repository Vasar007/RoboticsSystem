#ifndef PARSING_INL
#define PARSING_INL


template <class Container>
Container split(const std::string& str, const std::string_view delims) noexcept
{
    std::size_t previous	= 0;
    std::size_t current		= str.find_first_of(delims);
    if (current == std::string::npos)
    {
        return Container{};
    }

    std::size_t counter     = 0;
    for (const auto& ch : delims)
    {
        counter += std::count(str.begin(), str.end(), ch);
    }

    Container cont;
    cont.reserve(counter + 1);
    while (current != std::string::npos)
    {
        cont.emplace_back(str.substr(previous, current - previous));
        previous	= current + 1;
        current		= str.find_first_of(delims, previous);
    }

    cont.emplace_back(str.substr(previous, current - previous));

    return cont;
}

template <class Container>
Container fsplit(const std::string& str, const std::string_view delim) noexcept
{
    const std::size_t length    = delim.size();
    std::size_t previous        = 0;
    std::size_t current         = str.find(delim);
    if (current == std::string::npos)
    {
        return Container{};
    }

    std::size_t counter         = 0;
    const char* ptrToStr        = str.c_str();
    ptrToStr                    = strstr(ptrToStr, delim.data());
    while (ptrToStr != nullptr)
    {
        ++counter;
        ++ptrToStr;
        ptrToStr = strstr(ptrToStr, delim.data());
    }

    Container cont;
    cont.reserve(counter + 1);
    while (current != std::string::npos)
    {
        cont.emplace_back(str.substr(previous, current - previous));
        previous    = current + length;
        current     = str.find(delim, previous);
    }

    cont.emplace_back(str.substr(previous, current - previous));

    return cont;
}

#endif // PARSING_INL
