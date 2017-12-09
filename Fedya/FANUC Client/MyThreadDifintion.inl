#ifndef MODIFICATED_CLASS_DEFINITION
#define MODIFICATED_CLASS_DEFINITION
#pragma once

template <typename Fn, typename ... Args>
void MyThread::startThread(Fn&& fn, Args&&... arg)
{
	//finishing current thread
	stopThread();

	//starting new thread
	_flag = true;
	std::thread th(fn, &_mt, &_flag, arg...);
	_work.swap(th);
}

#endif
