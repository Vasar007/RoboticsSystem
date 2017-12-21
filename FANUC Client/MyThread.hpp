#ifndef MY_THREAD_H
#define MY_THREAD_H

#include<thread>
#include<mutex>

/**
 * \brief Class for working with parallel threads.
 */
class MyThread 
{
private:

	/**
	 * \brief Current thread.
	 */
	std::thread _work;
	/**
	 * \brief Mutex for locking current thread.
	 */
	std::mutex _mt;
	/**
	 * \brief Flag for finishing or continuing current thread.
	 */
	bool _flag = false;

public:

	/**
	 * \brief Default constructor.
	 */
	MyThread() = default;

	/**
	 * \brief           Constructor with starting new thread.
	 * \tparam Fn       Static function.
	 * \tparam Args     Parameters of static function.
	 * \param[in] fn    Link to static function.
	 * \param[in] arg   Parameters which take this static function.
	 */
	template <typename Fn, typename... Args>
	explicit MyThread(Fn&& fn, Args&&... arg);

	/**
	 * \brief               Method for swapping threads.
	 * \param[in] newThread Thread for swapping.
	 */
	void swap(std::thread& newThread);

	/**
	 * \brief               Method for swapping threads.
	 * \param[in] newThread MyThread for swapping.
	 */
	void swap(MyThread& newThread) noexcept;

	/**
	 * \brief    Method for checking if this thread may be joined.
	 * \return  True if this thread may be joined, else false.
	 */
	bool joinable() const;

	/**
	 * \brief           Method for launching new thread.
	 * \tparam Fn       Static function.
	 * \tparam Args     Parameters of static function.
	 * \param[in] fn    Link to static function.
	 * \param[in] arg   Parameters which take this static function.
	 */
	template <typename Fn, typename... Args>
	void startThread(Fn&& fn, Args&&... arg);

	/**
	 * \brief Method for closing current thread.
	 */
	void closeThread();

	/**
	 * \brief Default destructor.
	 */
	~MyThread();
};

#include "MyThread.inl"

#endif // MY_THREAD_H
