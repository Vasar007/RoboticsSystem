#pragma once

#ifndef MY_SHOWER

#define MY_SHOWER
#include "StaticField.hpp"
#include "MyThread.hpp"
#include <map>
#include <mutex>
#include <vector>

/**
* \brief Namespace of thread protected conosole inteface.
*/
namespace myInterface 
{
	/**
	 * \brief Singlton of debug inteface.
	 */
	class MyShower
	{
		/**
		 * \brief Vector of logs for showing.
		 */
		std::vector<Message> _logs;

		/**
		 * \brief Map for sorring fields.
		 */
		std::map<int, Message*> _list;
		
		/**
		 * \brief Handle of console (only for windows).
		 */
		HANDLE _hConsole;

		/**
		 * \brief Flag if interface need to be fully reoutputted.
		 */
		bool _needFullUpdate;
		
		/**
		 * \brief Mutex for locking thread.
		 */
		std::mutex _mt;

		/**
		 * \brief Number of next unused field.
		 */
		int _nextFreeField = 0;

		/**
		 * \brief Thread for paralel showing interface.
		 */
		MyThread _paralelShower;

		/**
		 * \brief Static function for paralel updating interface.
		 * \param mt Mutex for locking thread.
		 * \param flag Flag for ending this thread.
		 * \param instance Pointer to instance of this class.
		 */
		static void paralelShower(std::mutex* mt, bool* flag, MyShower* instance);

		/**
		 * \brief Default constructor.
		 */
		MyShower();

		/**
		 * \brief Deleted constructor.
		 */
		MyShower(const MyShower&) = delete;
		
		/**
		 * \brief Deleted constructor.
		 */
		MyShower(const MyShower&&) = delete;

		/**
		 * \brief Deleted constructor.
		 * \return
		 */
		MyShower& operator = (MyShower &) = delete;

		/**
		 * \brief Delted constructor.
		 * \return 
		 */
		MyShower& operator = (MyShower &&) = delete;

	public:

		/**
		 * \brief Method for adding new field.
		 * \param line Link to new field.
		 * \return Number of this link in storrage.
		 */
		int addField(Message* line);

		/**
		 * \brief Method for adding new log.
		 * \param str Discription of this log.
		 */
		void addLog(std::string str);

		/**
		 * \brief Method for adding new log.
		 * \tparam T Type of information for showing.
		 * \param str Discription of this log.
		 * \param obj Information for showing.
		 */
		template<typename T>
		void addLog(std::string str, T obj);

		/**
		 * \brief Method for deleting all logs.
		 */
		void clearLog();

		/**
		 * \brief Static Method for getting instance of this class.
		 * \return Instance of this class.
		 */
		static MyShower& getInstance();

		/**
		 * \brief Method for deleting field by it index.
		 * \param fieldId Id of field.
		 */
		void deleteField(int fieldId);

		~MyShower();
	};
}

/*ToDo
 * Case of adding and removing too many fields.
 * Reverse logs.
 * Adding controlling for supervisor.
 */

#include "MyShowerDifinition.inl"

#endif