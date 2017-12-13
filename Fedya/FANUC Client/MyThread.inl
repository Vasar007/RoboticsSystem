#ifndef MY_THREAD_INL
#define MY_THREAD_INL

template <typename Fn, typename ... Args>
MyThread::MyThread(Fn&& fn, Args&&... arg)
{
    startThread(fn, arg);
}

template <typename Fn, typename ... Args>
void MyThread::startThread(Fn&& fn, Args&&... arg)
{
	// finishing current thread
	closeThread();

	// starting new thread
	_flag = true;
	std::thread th(fn, &_mt, &_flag, arg...);
	_work.swap(th);
}

#endif // MY_THREAD_INL
