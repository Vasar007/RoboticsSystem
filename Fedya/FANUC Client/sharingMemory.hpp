#pragma once

#ifndef SHARING_MEMORY_MODEL

#include<Windows.h>
#include"MyQueue.hpp"
#include"SharingMemoryRead.hpp"
#include"SharingMemoryWrite.hpp"

#define SHARING_MEMORY_MODEL

//паралельное чтение

namespace usingRAM 
{
	template<typename T>
	struct SharingMemory 
	{
		MemorySharingRead<T> _insertBuffer;
		MemorySharingWrite<T> _outputBuffer;

		SharingMemory(LPCWSTR insertBufferName, LPCWSTR outputBufferName, MyQueue<T> *inputQueue, MyQueue<T> *outputQueue,
			int size, std::string inputBufferComment = "RAM", std::string outputBufferComment = "RAM") :
			_insertBuffer(insertBufferName, inputQueue, size, inputBufferComment),
			_outputBuffer(outputBufferName, outputQueue, size, outputBufferComment)
		{
		}

		~SharingMemory() = default;
	};///обьединнение классов для работ с буфером

}

#endif // !SHARING_MEMORY_MODEL