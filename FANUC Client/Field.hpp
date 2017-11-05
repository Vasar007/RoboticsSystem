#pragma once

#ifndef MY_FIELD

#define MY_FIELD
#include <string>
#include <vector>
#include <Windows.h>
#include <iostream>

namespace myInterface 
{
	//струтура одной строки интерфейса
	//текст и (число или строка из массива строк)
	struct Field
	{
		std::string _mes;
		int *_val;
		std::vector<std::string> _param;

		Field() = default;
		
		Field(std::string mes, int *num) : _mes(mes), _val(num)
		{
		}

		Field(std::string mes, int *num, std::vector<std::string> var) : _mes(mes), _val(num), _param(var)
		{
		}

		//функция быстрого обовления параметра
		void showQuick(HANDLE hconsole, int line) const
		{
			COORD coord;
			coord.X = _mes.size() + 1;
			coord.Y = line;
			SetConsoleCursorPosition(hconsole, coord);
			if (_param.size() == 0)
				printf("%d         ", *_val);
			else if (*_val >= 0 && *_val <= _param.size())
				printf("%s         ", _param[*_val].c_str());
			else
				printf("-1              ");

		}

		//функция обновления строки
		void show() const
		{
			if (_param.size() == 0)
				std::cout << _mes << " " << *_val << "\n";
			else if (*_val >= 0 && *_val <= _param.size())
				std::cout << _mes << " " << _param[*_val] << "\n";
			else
				std::cout << _mes << " -1\n";
		}
	};
}
//*/
#endif