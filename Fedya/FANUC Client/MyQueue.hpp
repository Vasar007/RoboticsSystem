#pragma once

#ifndef MY_QUEUE_DEF

#define MY_QUEUE_DEF

#include<queue>
#include<mutex>
#include"StaticField.hpp"

/**
 * \brief Class which impliment a queue for multithreading and time measurement.
 * \tparam T Object in queue.
 */
template<typename T>
class MyQueue
{
	/**
	 * \brief Basic queue with elements.
	 */
	std::queue<std::pair<clock_t,T>> _q;
	
	/**
	 * \brief Mutex for locking threads.
	 */
	std::mutex _mt;

	/**
	 * \brief Size of queue for interface.
	 */
	myInterface::StaticField<int> _sizeOfQueue{"Size of queue: ",0};
	
	/**
	 * \brief Time differnce of last erased element betwen adding and erasing elemnt in queue.
	 */
	myInterface::StaticField<int> _timeDifference{"Time delay in queue: ",0};
	
public:
	/**
	 * \brief Contructor of queue.
	 * \param comment Discription of this queue which will show in interface. 
	 * If it isn't specify this queue would be with default.
	 */
	explicit MyQueue(std::string comment);

	explicit MyQueue() = default;

	/**
	 * \brief Function for adding element.
	 * \param elem Element for adding.
	 */
	void push(T elem);

	/**
	 * \brief Fuction for checking if this queue is empty.
	 * \return True if epmty, else false.
	 */
	bool empty();

	/**
	 * \brief Function for checking size of queue.
	 * \return Retrun a size of queue.
	 */
	int size();

	/**
	 * \brief Function for getting time when front element was added.
	 * \return Return time of adding front element.
	 */
	clock_t frontTime();

	/**
	 * \brief Function for getting front element.
	 * \return Front element.
	 */
	T front();

	/**
	 * \brief Erase front element.
	 */
	void pop();

	/**
	 * \brief function for swapping queues
	 * \param q MyQueue for swapping
	 */
	void swap(MyQueue& q) noexcept;

	/**
	 * \brief Function which trys to erase ans return front element.
	 * \return True and object if there was front element or false and defult object if there wasn't front object.
	 */
	std::pair<bool, T> tryPull();

	/**
	 * \brief Default destructor.
	 */
	~MyQueue();
};

#include "MyQueueDifintion.inl"

#endif // !MY_QUEUE_DEF
