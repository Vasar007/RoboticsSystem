#pragma once

#ifndef SHARING_MEMORY_WRITE

#define SHARING_MEMORY_WRITE

#include <Windows.h>
#include "MyThread.hpp"
#include "MyQueue.hpp"
#include "MyShower.hpp"

namespace usingRAM 
{
	template<typename T>
	class MemorySharingWrite
	{
		HANDLE _hFileMemory;
		MyThread _th;
		int _numberOfPoints, _intefaceFieldId;

		static void paralelWrite(std::mutex *mt, bool *f, std::pair<char, T> *sPos, int size, MyQueue<T> *q, int tmpPos, int *numberOfPoints)
		{
			int tmp = 0;
			while (0 == 0) {
				if (tmp >= 100) {
					mt->lock();
					if (!*f) {
						mt->unlock();
						return;
					}
					mt->unlock();
					tmp = 0;
				}//выход
				tmp++;
				//cout << !(*Q).empty() << " " << (int)sPos[tmpPos].controler << endl;
				if (!q->empty() && sPos[tmpPos].first == 0 && sPos[(tmpPos + 1) % size].first == 0) {
					if (q->empty())
						continue;
					T corCoord = q->front();
					q->pop();
					sPos[tmpPos].second = corCoord;
					sPos[tmpPos].first = char(1);
					tmpPos = (tmpPos + 1) % size;
					++(*numberOfPoints);
				}
#ifdef LOCAL_DEBUG
				int tttmp = 0;
				static bool flag = false;
				if (!(*Q).empty() && sPos[tmpPos].first == 1) {
					tttmp++;
					flag = true;
				}
				if ((*Q).empty() && flag == true) {
					cout << "good" << tttmp << endl;
					tttmp = 0;
					flag = false;
				}
#endif  //LOCAL_DEBUG


			}
		}
	public:
		int _size = 0;
		MemorySharingWrite(LPCWSTR name, MyQueue<T> *q, int size = 128, std::string comment = "RAM", int physMemory = 8192)
		{
			_numberOfPoints = 0;
			if (sizeof(std::pair<char, T>)*_size + 1 > physMemory) {
				std::exception exp("number of coordinates needs more memory than in bufer");
				throw exp;
			}
			SetLastError(static_cast<DWORD>(0));
			_hFileMemory = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, physMemory, name);
			if (_hFileMemory == nullptr) {
				std::cout << GetLastError() << " " << ERROR_ALREADY_EXISTS << std::endl;
				return;
			}//позключение к буферу
			_size = size;

			std::pair<char, T> *m = static_cast<std::pair<char, T>*>(MapViewOfFile(_hFileMemory, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0));

			int curPos = 0;
			if (m[curPos].first == 0) {
				while (curPos < size && m[curPos].first == 0) {
					curPos++;
				}
				if (curPos == size) {
					curPos = 0;
				}
				else {
					while (curPos < size && m[curPos].first == 1) {
						curPos++;
					}
					if (curPos == size)
						curPos = 0;
				}
			}
			else {
				while (curPos < size && m[curPos].first == 1) {
					curPos++;
				}
				if (curPos == size)
					curPos = 0;
			}//нахождение позиции

			_th.startThread(paralelWrite, m, size, q, curPos, &_numberOfPoints);

			_intefaceFieldId = myInterface::MyShower::getInstance().addField(
				myInterface::Field(std::string("Number of pockets send to ") + comment + ": ", &_numberOfPoints));

		}
		~MemorySharingWrite()
		{
			_th.join();
			CloseHandle(_hFileMemory);
			myInterface::MyShower::getInstance().deleteField(_intefaceFieldId);
		}
	};
}

#endif
