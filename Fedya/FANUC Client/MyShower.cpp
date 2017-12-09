#include "MyShower.hpp"
#include <iostream>


void myInterface::MyShower::parallelShower(std::mutex* mt, bool* flag, MyShower* instance)
{
	int logsShown = 0;

	COORD coord;
	coord.X = 0;

	while (true)
	{
		mt->lock();
		if (!*flag)
		{
			mt->unlock();
			break;
		}
		mt->unlock();
		instance->_mt.lock();
		int curLine = 0;
		if (instance->_needFullUpdate)
		{
			system("cls");

			for (auto& it : instance->_fields)
			{
				it.second->show();
				++curLine;
			}

			++curLine;
			coord.Y = static_cast<short>(curLine);
			SetConsoleCursorPosition(instance->_hConsole, coord);

			std::cout << "Logs:\n";

			logsShown = 0;

			for (auto& it : instance->_logs)
			{
				it.show();
				++logsShown;
			}

			instance->_needFullUpdate = false;
		}
		else
		{
			_CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
			GetConsoleScreenBufferInfo(instance->_hConsole, &consoleInfo);

			for (const auto& it : instance->_fields)
			{
				it.second->showQuick(instance->_hConsole, curLine);
				curLine += static_cast<int>(it.second->size()) / consoleInfo.dwSize.X + 1;
			}

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

myInterface::MyShower::MyShower() : _needFullUpdate(true)
{
	_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	_parallelShower.startThread(parallelShower, this);
}

int myInterface::MyShower::addField(Message* line)
{
	_mt.lock();
	_fields.insert(std::make_pair(++_nextFreeField, line));
	//_list[++_lineNumber] = line;
	_needFullUpdate = true;
	_mt.unlock();
	return _nextFreeField;
}

void myInterface::MyShower::addLog(std::string str)
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
	_fields.clear();
	_logs.clear();
	_parallelShower.stopThread();
}
