#pragma once

#ifndef MY_QUEUE_DEF

#define MY_QUEUE_DEF

#include<queue>
#include<mutex>

/**
 * \brief class which impliment a queue for multithreading and time measurement
 * \tparam T object in queue
 */
template<typename T>
class MyQueue
{
	/**
	 * \brief basic queue with elements
	 */
	std::queue<std::pair<clock_t,T>> _q;
	
	/**
	 * \brief mutex for locking threads
	 */
	std::mutex _mt;

	/**
	 * \brief size of queue for interface
	 */
	int _sizeOfQueue{ 0 };
	
	/**
	 * \brief time differnce of last erased element betwen adding and erasing elemnt in queue
	 */
	int _timeDifference{ 0 };
	
	/**
	 * \brief id of first interface field
	 */
	int _interfaceId1;
	
	/**
	 * \brief if of secind interface id
	 */
	int _interfaceId2;
	
	/**
	 * \brief flag if this queue witg interface or without
	 */
	bool _withInterface;
public:
	/**
	 * \brief contructor of queue
	 * \param comment discription of this queue which will show in interface if it isn't specify this queue would be without interface
	 */
	explicit MyQueue(std::string comment = "without interface");

	/**
	 * \brief function for adding element
	 * \param elem element for adding
	 */
	void push(T elem);

	/**
	 * \brief fuction for checking if this queue is empty
	 * \return return true if epmty, else false
	 */
	bool empty();

	/**
	 * \brief function for checking size of queue
	 * \return retrun a size of queue
	 */
	int size();

	/**
	 * \brief function for getting time when front element was added
	 * \return return time of adding front element
	 */
	clock_t frontTime();

	/**
	 * \brief function for getting front element
	 * \return front element
	 */
	T front();

	/**
	 * \brief erase front element
	 */
	void pop();

	/**
	 * \brief function for swapping queues
	 * \param q MyQueue for swapping
	 */
	void swap(MyQueue& q) noexcept;

	/**
	 * \brief function which trys to erase ans return front element
	 * \return true and object if there was front element or false and defult object if there wasn't front object
	 */
	std::pair<bool, T> pull();

	/**
	 * \brief default destructor
	 */
	~MyQueue();
};

#include "MyQueueDifintion.hpp"

#endif // !MY_QUEUE_DEF
