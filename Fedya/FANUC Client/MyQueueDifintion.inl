#ifndef MY_QUEUE_DIFINITION
#define MY_QUEUE_DIFINITION
#pragma once

#include "MyShower.hpp"

template <typename T>
MyQueue<T>::MyQueue(std::string comment):_sizeOfQueue("Size of " + comment,0),_timeDifference("Time delay in " + comment,0)
{
}

template <typename T>
void MyQueue<T>::push(T elem)
{
	clock_t t = clock();
	_mt.lock();
	_q.push(std::make_pair(t, elem));
	_mt.unlock();
	_sizeOfQueue.setObject(_sizeOfQueue.getObject()+1);
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
	_timeDifference.setObject(static_cast<int>(clock()) - static_cast<int>(_q.front().first));
	_q.pop();
	_sizeOfQueue.setObject(_sizeOfQueue.getObject()-1);
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
		_sizeOfQueue.setObject(_sizeOfQueue.getObject() - 1);
	}
	_mt.unlock();
	return ans;
}

template <typename T>
MyQueue<T>::~MyQueue()
{
	while (!_q.empty())
		_q.pop();
}

#endif
