#ifndef CONFIG_INL
#define CONFIG_INL


template <class T>
T NamedConfig::get(const std::string_view paramName) const
{
    if (const auto result = _storage.find(paramName); result != _storage.end())
    {
        const bool typeChecking = result->second.type() == typeid(T);
        if (!typeChecking)
        {
            std::cout << "\n\n" << result->second.type().name() << " IS NOT "
                << typeid(T).name() << "\n\n";
        }
        assert(typeChecking);
        return std::any_cast<T>(result->second);
    }

    assert(false);
    return T{};
}

#endif // CONFIG_INL
