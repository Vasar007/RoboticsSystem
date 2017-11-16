#pragma once

#ifndef MY_SHOWER

#define MY_SHOWER
#include "StaticField.hpp"
#include "MyThread.hpp"
#include <map>
#include <mutex>
#include <vector>

namespace myInterface 
{
	//синглтон интерфейса
	class MyShower
	{
		std::vector<Message> _logs;

		std::map<int, Message> _list;
		
		HANDLE _hConsole;

		bool _needFullUpdate;
		
		std::mutex _mt;

		int _lineNumber = 0;

		MyThread _paralelShower;

		static void paralelShower(std::mutex *mt, bool *flag, MyShower *instance)
		{
			int logsShown = 0;

			COORD coord;
			coord.X = 0;

			while(true)
			{
				mt->lock();
				if(!*flag)
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

					for (auto& it : instance->_list)
					{
						it.second.show();
						++curLine;
					}

					++curLine;
					coord.Y = curLine++;
					SetConsoleCursorPosition(instance->_hConsole, coord);

					std::cout << "Logs:\n";

					logsShown = 0;

					for (auto& it : instance->_logs)
					{
						it.show();
						++curLine;
						++logsShown;
					}

					instance->_needFullUpdate = false;
				}
				else 
				{
					for (auto& it : instance->_list)
					{
						it.second.showQuick(instance->_hConsole, curLine++);
					}

					coord.Y = curLine + 2 + logsShown;
					SetConsoleCursorPosition(instance->_hConsole, coord);


					for(size_t i = logsShown;i<instance->_logs.size();i++)
					{
						instance->_logs[i].show();
					}
				}
				
				instance->_mt.unlock();

				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}

		MyShower(): _needFullUpdate(true)
		{
			_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			_paralelShower.startThread(paralelShower, this);
		}

		MyShower(const MyShower&) = delete;
		
		MyShower(const MyShower&&) = delete;

		MyShower& operator = (MyShower &) = delete;

		MyShower& operator = (MyShower &&) = delete;

	public:

		//метод добавления строки в интерфейс
		int addLine(Message& line)
		{
			_mt.lock();
			_list.insert( std::make_pair( ++_lineNumber ,line ));
			//_list[++_lineNumber] = line;
			_needFullUpdate = true;
			_mt.unlock();
			return _lineNumber;
		}

		void showLog(std::string str)
		{
			_logs.emplace_back(Field<std::string>("+ " + str,""));
		}

		//метод вывода дополнительной информации
		template<typename T>
		void showLog(std::string str,T obj)
		{
			_logs.emplace_back(Field<T>("+ " + str,obj));
		}

		void clearLog()
		{
			_logs.clear();
		}

		//метод возвращающий реализацию класса
		static MyShower& getInstance()
		{
			static MyShower instance;
			return instance;
		}

		//метод удаления поля по индексу
		void deleteLine(int fieldId)
		{
			_mt.lock();
			_list.erase(fieldId);
			_needFullUpdate = true;
			_mt.unlock();
		}

		~MyShower()
		{
			_list.clear();
			_logs.clear();
			_paralelShower.join();
		}
	};
}

#include "MyShowerDifinition.inl"

#endif