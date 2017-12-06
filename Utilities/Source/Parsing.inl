#ifndef PARSING_INL
#define PARSING_INL


template <class Container>
void split(const std::string& str, Container& cont, const std::string_view delims) noexcept
{
	std::size_t previous	= 0u;
	std::size_t current		= str.find_first_of(delims);

	while (current != std::string::npos)
	{
		cont.push_back(str.substr(previous, current - previous));
		previous	= current + 1u;
		current		= str.find_first_of(delims, previous);
	}

	cont.push_back(str.substr(previous, current - previous));
}

template <class Container>
void fsplit(const std::string& str, Container& cont, const std::string_view delim) noexcept
{
	const std::size_t length	= delim.size();
	std::size_t previous		= 0u;
	std::size_t current			= str.find(delim);

	while (current != std::string::npos)
	{
		cont.push_back(str.substr(previous, current - previous - length));
		previous	= current + length;
		current		= str.find(delim, previous);
	}
}

#endif // PARSING_INL
