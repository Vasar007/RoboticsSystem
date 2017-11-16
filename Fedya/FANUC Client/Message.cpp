#include "Message.hpp"

#include <iostream>
#include "MyShower.hpp"

myInterface::Message::Message(std::string discription, std::string message = "")
: _discription(discription)
, _message(message)
{
}

void myInterface::Message::showQuick(HANDLE hConsole, int line)
{
	if (_needUpdate) {

		COORD coord;
		coord.X = _message.size();
		coord.Y = line;
		
		SetConsoleCursorPosition(hConsole, coord);
		
		std::cout << _message<<"          ";
		
		_needUpdate = false;
	}
}

void myInterface::Message::show()
{
	std::cout<< _discription << ' ' << _message << '\n';
	
	_needUpdate = false;
}

