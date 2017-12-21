#include <iostream>

#include "MyShower.hpp"

void myInterface::MyShower::parallelShower(std::mutex* mt, bool* flag, MyShower* instance)
{
    //  Nummber of shown logs.
    int logsShown = 0;

    // Struct for setting new cursor position.
	COORD coord;
	coord.X = 0;

	while (true)
	{
        // Ending endless cycle.
		mt->lock();
		if (!*flag)
		{
			mt->unlock();
			break;
		}
		mt->unlock();
		instance->_mt.lock();

        // Current outputting line.
	    int curLine = 0;
		
	    if (instance->_needFullUpdate)
		{
            // Cleaning console.
			system("cls");

            // Outputting fields.
			for (auto& it : instance->_fields)
			{
				it.second->show();
				++curLine;
			}

            // Changing cursor position.
			++curLine;
			coord.Y = static_cast<short>(curLine);
			SetConsoleCursorPosition(instance->_hConsole, coord);

            // Outputting interface.
			std::cout << "Logs:\n";

            // Setting number of shown logs to zero.
			logsShown = 0;

            // outputting logs.
			for (auto& it : instance->_logs)
			{
				it.show();
				++logsShown;
			}

            // Changing to unupdate-able mode.
			instance->_needFullUpdate = false;
		}
		else
		{
            // Getting console info.
			_CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
			GetConsoleScreenBufferInfo(instance->_hConsole, &consoleInfo);

            // Updating fields.
			for (const auto& it : instance->_fields)
			{
				it.second->showQuick(instance->_hConsole, curLine);
				curLine += static_cast<int>(it.second->size()) / consoleInfo.dwSize.X + 1;
			}

            // outputrring logs.
			if (instance->_logs.size() >= 10u)
			{
				coord.Y = static_cast<short>(curLine + 2);
				SetConsoleCursorPosition(instance->_hConsole, coord);

				logsShown = 0;

				for (auto& it : instance->_logs)
				{
					it.show();
					++logsShown;
				}
			}
			else
			{
				coord.Y = static_cast<short>(curLine + 2 + logsShown);
				SetConsoleCursorPosition(instance->_hConsole, coord);

				for (size_t i = logsShown; i < instance->_logs.size(); ++i)
				{
					instance->_logs[i].show();
					++logsShown;
				}
			}
		}

		instance->_mt.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

myInterface::MyShower::MyShower() 
    : _needFullUpdate(true)
{
	_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	_parallelShower.startThread(parallelShower, this);
}

int myInterface::MyShower::addField(Message* line)
{
	_mt.lock();
	_fields.insert(std::make_pair(++_nextFreeField, line));
	_needFullUpdate = true;
	_mt.unlock();
	return _nextFreeField;
}

void myInterface::MyShower::addLog(const std::string& str)
{
	_logs.emplace_back(Field<std::string>("+ " + str, ""));
	if (_logs.size() > 10u)
	{
		_logs.erase(_logs.begin());
	}
}

void myInterface::MyShower::clearLog()
{
	_logs.clear();
}

myInterface::MyShower& myInterface::MyShower::getInstance()
{
	static MyShower instance;
	return instance;
}

void myInterface::MyShower::deleteField(int fieldId)
{
	_mt.lock();
	_fields.erase(fieldId);
	_needFullUpdate = true;
	_mt.unlock();
}

myInterface::MyShower::~MyShower()
{
	_parallelShower.closeThread();
}
