#ifndef MY_QUEUE_INL
#define MY_QUEUE_INL

#include "MyShower.hpp"

template <typename T>
MyQueue<T>::MyQueue(const std::string& comment)
    : _sizeOfQueue("Size of \"" + comment + "\": ",0),
      _timeDifference("Time delay in \"" + comment + "\": ",0)
{
}

template <typename T>
void MyQueue<T>::push(const T elem)
{
    // Get current time.
	clock_t t = clock();
	
    // Adding new elment.
    _mt.lock();
	_q.push(std::make_pair(t, elem));
	_mt.unlock();

    // Changing information in field.
	_sizeOfQueue.setObject(_sizeOfQueue.getObject()+1);
}

template <typename T>
bool MyQueue<T>::isEmpty()
{
	_mt.lock();
	const bool ans = _q.empty();
	_mt.unlock();
	return ans;
}

template <typename T>
size_t MyQueue<T>::size()
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
std::pair<bool, T> MyQueue<T>::tryPull()
{
	std::pair<bool, T> ans;
	_mt.lock();
	if (_q.empty())
	{
		ans.first = false;
	}
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

#endif // MY_QUEUE_INL
