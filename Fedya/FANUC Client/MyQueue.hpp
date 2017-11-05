#pragma once

#ifndef MY_QUEUE_DEF

#define MY_QUEUE_DEF

#include<queue>
#include<mutex>

//обртка над очередью
template<typename T>
class MyQueue
{
	std::queue<std::pair<int,T>> _q;
	std::mutex _mt;
	int _sizeOfQueue, _timeDifference,_interfaceId1,_interfaceId2;
	bool _withInterface;
public:
	explicit MyQueue(std::string comment = "MyQueue")
	{
		_sizeOfQueue = 0;
		_timeDifference = 0;
		_withInterface = comment != "without interface";
		if (_withInterface) 
		{
			_interfaceId1 = myInterface::MyShower::getInstance().addField(
				myInterface::Field(std::string("size of ") + comment + ": ", &_sizeOfQueue));
			_interfaceId2 = myInterface::MyShower::getInstance().addField(
				myInterface::Field(std::string("time difference betwen inserting and erasing point in ") + comment + ": ", &_timeDifference));
		}
	}

	//метод добавления элемента в очередь
	void push(T elem)
	{
		int t = clock();
		_mt.lock();
		_q.push(std::make_pair(t,elem));
		_mt.unlock();
		++_sizeOfQueue;
	}

	//метод прверки на пустоту
	bool empty()
	{
		_mt.lock();
		const bool ans = _q.empty();
		_mt.unlock();
		return ans;
	}

	//метод получения размера
	int size()
	{
		_mt.lock();
		const int ans = _q.size();
		_mt.unlock();
		return ans;
	}

	//метод получения временной отметки первой точки
	int frontTime()
	{
		_mt.lock();
		const int ans = _q.front().first;
		_mt.unlock();
		return ans;
	}

	//метод получения первого элемента
	T front()
	{
		_mt.lock();
		const T elem = _q.front().second;
		_mt.unlock();
		return elem;
	}

	//метод удаления первого элемента
	void pop()
	{
		_mt.lock();
		_timeDifference = clock() - _q.front().first;
		_q.pop();
		--_sizeOfQueue;
		_mt.unlock();
	}

	//метод обмена очередями
	void swap(MyQueue& q) noexcept
	{
		_mt.lock();
		q._mt.lock();
		_q.swap(q._q);
		q._mt.unlock();
		_mt.unlock();
	}

	//сетод пытающий удалить первый элемент с последующим его удалением
	std::pair<bool, T> pull()
	{
		std::pair<bool, T> ans;
		_mt.lock();
		if (_q.empty())
			ans.first = false;
		else 
		{
			ans.first = true;
			ans.second = _q.front().second;
			_q.pop();
			--_sizeOfQueue;
		}
		_mt.unlock();
		return ans;
	}
	~MyQueue()
	{
		while (!_q.empty())
			_q.pop();
		if (_withInterface) 
		{
			myInterface::MyShower::getInstance().deleteField(_interfaceId1);
			myInterface::MyShower::getInstance().deleteField(_interfaceId2);
		}
	}
};

#endif // !MY_QUEUE_DEF
