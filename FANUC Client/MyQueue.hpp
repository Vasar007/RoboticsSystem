#ifndef MY_QUEUE_H
#define MY_QUEUE_H

#include<queue>
#include<mutex>
#include <chrono>

#include"StaticField.hpp"

/**
 * \brief       Class which implement a multi-thread-protected queue and time measurement.
 * \tparam T    Type of object in queue.
 */
template<typename T>
class MyQueue
{
private:

	/**
	 * \brief Basic queue with elements.
	 */
	std::queue<std::pair<std::chrono::high_resolution_clock::time_point,T>> _q;
	
	/**
	 * \brief Mutex for locking threads.
	 */
	std::mutex _mt;

	/**
	 * \brief Interface field with size of queue.
	 */
	myInterface::StaticField<int> _sizeOfQueue{"Size of queue: ",0};
	
	/**
	 * \brief Time difference of last erased element between adding and erasing element in queue.
	 */
	myInterface::StaticField<int> _timeDifference{"Time delay in queue: ",0};
	
public:

	/**
	 * \brief               Contructor of queue.
	 * \param[in] comment   Description of this queue which will show in interface. 
	 *                      If it isn't specify this queue would be with default description.
	 */
	explicit MyQueue(const std::string& comment);

    /**
	 * \brief Constructor.
	 */
	explicit MyQueue() = default;

	/**
	 * \brief               Method for adding element.
	 * \param[in] object    Element for adding.
	 */
	void push(const T object);

	/**
	 * \brief   Method for checking if this queue is empty.
	 * \return  True if empty, else false.
	 */
	bool isEmpty();

	/**
	 * \brief   Method for getting size of queue.
	 * \return  Number of elements in queue.
	 */
	size_t size();

	/**
	 * \brief   Method for getting time, when front element was added.
	 * \return  Return time of adding front element.
	 */
    long long frontTime();

	/**
	 * \brief   Method for getting front element.
	 * \return  Front element.
	 */
	T front();

	/**
	 * \brief Method for erasing front element.
	 */
	void pop();

	/**
	 * \brief               Method for swapping queues.
	 * \param[in] newQueue  MyQueue for swapping.
	 */
	void swap(MyQueue& newQueue) noexcept;

	/**
	 * \brief   Method which tries to erase and return front element.
	 * \return  True and object if there was front element or
	 *          false and default object if there wasn't front object.
	 */
	std::pair<bool, T> tryPull();

	/**
	 * \brief Default destructor.
	 */
	~MyQueue() = default;
};

#include "MyQueue.inl"

#endif // MY_QUEUE_H
