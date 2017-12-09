#include "Message.hpp"

#include <iostream>
#include "MyShower.hpp"

myInterface::Message::Message(std::string discription, std::string message = "")
: _description(discription)
, _message(message)
{
}

void myInterface::Message::showQuick(HANDLE hConsole, int line)
{
	if (_needUpdate) {

		COORD coord;
		coord.X = static_cast<short>(_description.size());
		coord.Y = static_cast<short>(line);
		
		SetConsoleCursorPosition(hConsole, coord);
		
		std::cout << _message<<"          ";
		
		_needUpdate = false;
	}
}

void myInterface::Message::show()
{
	std::cout<< _description << ' ' << _message << '\n';
	
	_needUpdate = false;
}

size_t myInterface::Message::size() const
{
	return _description.size() + _message.size() + 1;
}

