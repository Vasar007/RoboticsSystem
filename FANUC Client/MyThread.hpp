#pragma once


#ifndef MY_THREAD_DEF

#define MY_THREAD_DEF

#include<thread>
#include<mutex>

//обертка над потоками
struct MyThread 
{
	std::thread _work;
	std::mutex _mt;
	bool _flag = true;

	MyThread() = default;

	template <typename Fn, typename... Args>
	explicit MyThread(Fn&& fn, Args&&... arg)
	{
		_flag = true;
		std::thread th(fn, &_mt, &_flag, arg...);
		_work.swap(th);
	}

	//обмен потоков
	void swap(std::thread& th)
	{
		_work.swap(th);
	}

	//обмен потоков
	void swap(MyThread& th) noexcept
	{
		swap(th._work);
	}

	//проверка на возможность синхронизации
	bool joinable() const
	{
		return _work.joinable();
	}

	//запуск нового потока
	template <typename Fn, typename... Args>
	void startThread(Fn&& fn, Args&&... arg)
	{
		join();
		_flag = true;
		std::thread th(fn, &_mt, &_flag, arg...);
		_work.swap(th);
	}

	//завершение нового потока
	void join()
	{
		if (joinable()) 
		{
			_mt.lock();
			_flag = false;
			_mt.unlock();
			_work.join();
		}
	}

	~MyThread()
	{
		join();
	}
};


#endif // !MyThread_def