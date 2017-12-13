#include <iostream>

#include "Message.hpp"
#include "MyShower.hpp"

myInterface::Message::Message(const std::string& discription,const std::string& message = "")
    : _description(discription),
      _message(message)
{
}

void myInterface::Message::showQuick(HANDLE hConsole, int line)
{
	if (_needUpdate)
	{
        // Struct for changing position of console cursor.
		COORD coord;
		coord.X = static_cast<short>(_description.size());
		coord.Y = static_cast<short>(line);
		
        // Changing position of console cursor.
		SetConsoleCursorPosition(hConsole, coord);
		
        // Outputting new information.
		std::cout << _message<<"          ";
		
        // Changing to unupdate-able status.
		_needUpdate = false;
	}
}

void myInterface::Message::show()
{
    // Outputting information. 
	std::cout<< _description << ' ' << _message << '\n';
	
	_needUpdate = false;
}

size_t myInterface::Message::size() const
{
	return _description.size() + _message.size() + 1;
}

