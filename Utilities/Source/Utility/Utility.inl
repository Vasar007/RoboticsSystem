#ifndef UTILITY_INL
#define UTILITY_INL


template <class T>
std::string toString(const T& value)
{
    std::stringstream stream{};
    stream << value;
    return stream.str();
}

template <class T>
T fromString(const std::string& str, bool& ok)
{
    std::stringstream ss(str);
    T t;

    ss >> t;
    ok = !ss.fail();
    return t;
}

template <>
inline std::string fromString(const std::string& str, bool& ok)
{
    ok = true;
    return str;
}

template <class T>
std::unique_ptr<T> copyUnique(const std::unique_ptr<T>& source)
{
    return source ? std::make_unique<T>(*source) : nullptr;
}

template <class InputIt1, class InputIt2, class T>
[[nodiscard]]
T distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, T value, const T& divisor)
{
    assert(divisor != 0);

    auto squareOfSumsOfDifferences = [&divisor](T a, T b)
    {
        return static_cast<T>((a - b) * (a - b) / (divisor * divisor));
    };
    const T result = std::inner_product(first1, last1, first2, value, std::plus<>(),
                                        squareOfSumsOfDifferences);
    return static_cast<T>(std::sqrt(result));
}

#endif // UTILITY_INL
