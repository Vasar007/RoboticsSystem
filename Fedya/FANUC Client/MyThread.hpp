#ifndef MY_THREAD_DEF
#define MY_THREAD_DEF
#pragma once

#include<thread>
#include<mutex>


/**
 * \brief Class for working threads.
 */
struct MyThread 
{
	/**
	 * \brief Current thread.
	 */
	std::thread _work;
	/**
	 * \brief Mutex for locking curretn thread.
	 */
	std::mutex _mt;
	/**
	 * \brief Flag for finishing or continuing current thread.
	 */
	bool _flag{ false };

	/**
	 * \brief Default constructor.
	 */
	MyThread() = default;

	/**
	 * \brief Constructor with starting new thread.
	 * \tparam Fn Static function.
	 * \tparam Args Params of static function.
	 * \param fn Link to static function.
	 * \param arg Params whick tale this staitc function.
	 */
	template <typename Fn, typename... Args>
	explicit MyThread(Fn&& fn, Args&&... arg);

	/**
	 * \brief Method for swapping threads.
	 * \param th Thread for swapping.
	 */
	void swap(std::thread& th);

	/**
	 * \brief Function for swapping threads.
	 * \param th MyThread for swapping.
	 */
	void swap(MyThread& th) noexcept;

	/**
	 * \brief Function for checking if this thread may be joind.
	 * \return True if this thread may be joined, else false.
	 */
	bool joinable() const;

	/**
	 * \brief Function for starting new thread.
	 * \tparam Fn Static function.
	 * \tparam Args Params of static function.
	 * \param fn Link to static function.
	 * \param arg Params whick tale this staitc function.
	 */
	template <typename Fn, typename... Args>
	void startThread(Fn&& fn, Args&&... arg);

	/**
	 * \brief Function for stopping current thread.
	 */
	void stopThread();

	/**
	 * \brief Default destructor.
	 */
	~MyThread();
};

#include "MyThreadDifintion.inl"

#endif // MyThread_def
