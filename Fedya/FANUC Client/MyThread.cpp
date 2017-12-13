#include"MyThread.hpp"

inline void MyThread::swap(std::thread& th)
{
	_work.swap(th);
}

inline void MyThread::swap(MyThread& th) noexcept
{
	swap(th._work);
}

bool MyThread::joinable() const
{
	return _work.joinable();
}

void MyThread::closeThread()
{
	if (joinable())
	{
		_mt.lock();
		_flag = false;
		_mt.unlock();
		_work.join();
	}
}

MyThread::~MyThread()
{
	closeThread();
}