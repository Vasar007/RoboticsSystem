#pragma once

#ifndef MEMORY_SHARING_READ

#define MEMORY_SHARING_READ

#include<Windows.h>
#include"MyThread.hpp"

namespace usingRAM 
{
	template<typename T>
	class MemorySharingRead 
	{
		HANDLE _hFileMemory;
		MyThread _th;
		int _numberOfPoints, _intefaceFieldId;

		static void paralelRead(std::mutex *mt, bool *f, MyQueue<T> *q, int size, std::pair<char, T> *m, int pos, int *numberOfPoints)
		{
			int tmp = 0;
			while (0 == 0) {
				if (tmp >= 20) {
					mt->lock();
					if (!*f) {
						mt->unlock();
						break;
					}
					mt->unlock();
					tmp = 0;
				}//выход из цикла
				if (m[pos].first == 1) {
					q->push(m[pos].second);
					m[(pos + size - 1) % size].first = 0;
					++(*numberOfPoints);
					//++numberOfCoordGetFromUser;
					pos = (pos + 1) % size;
				}
				tmp++;
			}
		}
	public:
		int _size = 0;
		MemorySharingRead(LPCWSTR name, MyQueue<T> *q, int size = 128, std::string comment = "RAM", int physMemory = 8192)
		{
			//name = "Global\\" + name;
			if (sizeof(std::pair<char, T>)*_size + 1 > physMemory) {
				std::exception exp("number of coordinates needs more memory than in bufer");
				throw exp;
			}
			SetLastError(static_cast<DWORD>(0));
			_hFileMemory = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, physMemory, name);
			if (_hFileMemory == nullptr) {
				std::cout << GetLastError() << " " << ERROR_ALREADY_EXISTS << std::endl;
				return;
			}//подключение к буферу
			_size = size;

			std::pair<char, T> *m = static_cast<std::pair<char, T>*>(MapViewOfFile(_hFileMemory, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0));

			int curPos = 0;
			if (m[curPos].first == 1) {
				while (curPos < size && m[curPos].first == 1) {
					curPos++;
				}
				if (curPos == size) {
					curPos = -1;
				}
				else {
					while (curPos < size && m[curPos].first == 0) {
						curPos++;
					}
					if (curPos == size)
						curPos = 0;
				}
			}
			else {
				while (curPos < size && m[curPos].first == 0) {
					curPos++;
				}
				if (curPos == size)
					curPos = -1;
			}
			curPos = (curPos + 1) % size;
			m[(curPos + size - 1) % size].first = 1;
			//нахождение позиции

			_numberOfPoints = 0;

			_th.startThread(paralelRead, q, size, m, curPos, &_numberOfPoints);

			_intefaceFieldId = myInterface::MyShower::getInstance().addField(
				myInterface::Field(std::string("Number of pockets recived from ") + comment + ": ", &_numberOfPoints));
		}
		~MemorySharingRead()
		{
			_th.join();
			CloseHandle(_hFileMemory);
			myInterface::MyShower::getInstance().deleteField(_intefaceFieldId);
		}
	};
}

#endif