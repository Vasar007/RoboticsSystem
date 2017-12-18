#ifndef UTILITY_INL
#define UTILITY_INL


template <class T, class U>
constexpr bool isSame() noexcept
{
	return std::is_same<T, U>::value;
}

template <typename T>
std::string toString(const T& value) noexcept
{
	std::stringstream stream;

	stream << value;

	return stream.str();
}

template <typename T> 
T fromString(const std::string& str, bool& ok) noexcept
{
	std::stringstream ss(str);
	T t;

	ss >> t;
	ok = !ss.fail();

	return t;
}

template <>
inline std::string fromString(const std::string& str, bool& ok) noexcept
{
	ok = true;
	return str;
}

template <typename T>
std::unique_ptr<T> copyUnique(const std::unique_ptr<T>& source) noexcept
{
	return source ? std::make_unique<T>(*source) : nullptr;
}

template <class T>
void swap(T& first, T& second) noexcept
{
	// Enable ADL (not necessary in our case, but good practice).
	using std::swap;

	if constexpr (isSame<T, Client>() || isSame<T, ServerImitator>())
	{
		swap(first._wsaData,				second._wsaData);
		swap(first._sendingSocket,			second._sendingSocket);
		swap(first._receivingSocket,		second._receivingSocket);
		swap(first._sendingSocketAddress,	second._sendingSocketAddress);
		swap(first._receivingSocketAddress,	second._receivingSocketAddress);
		swap(first._addressInfo,			second._addressInfo);
		swap(first._isRunning,				second._isRunning);
		swap(first._wasInitialized,			second._wasInitialized);
		swap(first._buffer,					second._buffer);
		swap(first._message,				second._message);
	}
	else
	{
		swap(first, second);
	}
}

#endif // UTILITY_INL
