#include"MyThread.hpp"

template <typename Fn, typename ... Args>
MyThread::MyThread(Fn&& fn, Args&&... arg)
{
	//starting new thread
	startThread(fn, arg);
}//не работает

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

void MyThread::stopThread()
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
	stopThread();
}//*/