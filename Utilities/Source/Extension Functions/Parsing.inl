#ifndef PARSING_INL
#define PARSING_INL


template <class Container>
Container split(const std::string& str, const std::string_view delims) noexcept
{
	Container cont;

	std::size_t previous	= 0u;
	std::size_t current		= str.find_first_of(delims);

	while (current != std::string::npos)
	{
		cont.push_back(str.substr(previous, current - previous));
		previous	= current + 1u;
		current		= str.find_first_of(delims, previous);
	}

	cont.push_back(str.substr(previous, current - previous));

	return cont;
}

template <class Container>
Container fsplit(const std::string& str, const std::string_view delim) noexcept
{
	Container cont;

	const std::size_t length	= delim.size();
	std::size_t previous		= 0u;
	std::size_t current			= str.find(delim);

	while (current != std::string::npos)
	{
		cont.push_back(str.substr(previous, current - previous - length));
		previous	= current + length;
		current		= str.find(delim, previous);
	}

	return cont;
}

#endif // PARSING_INL
