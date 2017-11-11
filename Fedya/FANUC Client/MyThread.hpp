#ifndef MY_THREAD_DEF
#define MY_THREAD_DEF
#pragma once

#include<thread>
#include<mutex>


/**
 * \brief class for working threads
 */
struct MyThread 
{
	/**
	 * \brief current thread
	 */
	std::thread _work;
	/**
	 * \brief mutex for locking curretn thread
	 */
	std::mutex _mt;
	/**
	 * \brief flag for finishing or continuing current thread
	 */
	bool _flag{ false };

	/**
	 * \brief default constructor
	 */
	MyThread() = default;

	/**
	 * \brief constructor with starting new thread
	 * \tparam Fn static function
	 * \tparam Args params of static function
	 * \param fn link to static function
	 * \param arg params whick tale this staitc function
	 */
	template <typename Fn, typename... Args>
	explicit MyThread(Fn&& fn, Args&&... arg);

	/**
	 * \brief method for swapping threads
	 * \param th thread for swapping
	 */
	void swap(std::thread& th);

	/**
	 * \brief function for swapping threads
	 * \param th thread for swapping
	 */
	void swap(MyThread& th) noexcept;

	/**
	 * \brief function for checking if this thread may be joind
	 * \return true if this thread may be joind, else false
	 */
	bool joinable() const;

	/**
	 * \brief function for starting new thread
	 * \tparam Fn static function
	 * \tparam Args params of static function
	 * \param fn link to static function
	 * \param arg params whick tale this staitc function
	 */
	template <typename Fn, typename... Args>
	void startThread(Fn&& fn, Args&&... arg);

	/**
	 * \brief function for stopping current thread
	 */
	void join();

	/**
	 * \brief default destructor
	 */
	~MyThread();
};




template <typename Fn, typename ... Args>
void MyThread::startThread(Fn&& fn, Args&&... arg)
{
	//finishing current thread
	join();

	//starting new thread
	_flag = true;
	std::thread th(fn, &_mt, &_flag, arg...);
	_work.swap(th);
}
//*/

#endif // MyThread_def
