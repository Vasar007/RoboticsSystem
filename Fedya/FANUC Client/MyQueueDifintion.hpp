#ifndef MY_QUEUE_DIFINITION
#define MY_QUEUE_DIFINITION
#pragma once

#include "MyShower.hpp"

template <typename T>
MyQueue<T>::MyQueue(std::string comment)
{
	_withInterface = comment != "without interface";
	if (_withInterface)
	{
		_interfaceId1 = myInterface::MyShower::getInstance().addField(
			myInterface::Field(std::string("size of ") + comment + ": ", &_sizeOfQueue));
		_interfaceId2 = myInterface::MyShower::getInstance().addField(
			myInterface::Field(std::string("time difference between inserting and erasing point in ") + comment + ": ",
				&_timeDifference));
	}
}

template <typename T>
void MyQueue<T>::push(T elem)
{
	clock_t t = clock();
	_mt.lock();
	_q.push(std::make_pair(t, elem));
	_mt.unlock();
	++_sizeOfQueue;
}

template <typename T>
bool MyQueue<T>::empty()
{
	_mt.lock();
	const bool ans = _q.empty();
	_mt.unlock();
	return ans;
}

template <typename T>
int MyQueue<T>::size()
{
	_mt.lock();
	const int ans = _q.size();
	_mt.unlock();
	return ans;
}

template <typename T>
clock_t MyQueue<T>::frontTime()
{
	_mt.lock();
	const int ans = _q.front().first;
	_mt.unlock();
	return ans;
}

template <typename T>
T MyQueue<T>::front()
{
	_mt.lock();
	const T elem = _q.front().second;
	_mt.unlock();
	return elem;
}

template <typename T>
void MyQueue<T>::pop()
{
	_mt.lock();
	_timeDifference = static_cast<int>(clock()) - static_cast<int>(_q.front().first);
	_q.pop();
	--_sizeOfQueue;
	_mt.unlock();
}

template <typename T>
void MyQueue<T>::swap(MyQueue& q) noexcept
{
	_mt.lock();
	q._mt.lock();
	_q.swap(q._q);
	q._mt.unlock();
	_mt.unlock();
}

template <typename T>
std::pair<bool, T> MyQueue<T>::pull()
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

template <typename T>
MyQueue<T>::~MyQueue()
{
	while (!_q.empty())
		_q.pop();
	if (_withInterface)
	{
		myInterface::MyShower::getInstance().deleteField(_interfaceId1);
		myInterface::MyShower::getInstance().deleteField(_interfaceId2);
	}
}

#endif
