#ifndef MY_SHOWER
#define MY_SHOWER
#pragma once

#include "StaticField.hpp"
#include "MyThread.hpp"
#include <map>
#include <mutex>
#include <vector>

/**
* \brief Namespace of thread protected console interface.
*/
namespace myInterface 
{
	/**
	 * \brief Singlton of debug interface.
	 */
	class MyShower
	{
		/**
		 * \brief Vector of logs for showing.
		 */
		std::vector<Message> _logs;

		/**
		 * \brief Map for showing fields.
		 */
		std::map<int, Message*> _fields;
		
		/**
		 * \brief Handle of console (only for windows).
		 */
		HANDLE _hConsole;

		/**
		 * \brief Flag if interface need to be fully re outputted.
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
		 * \brief Thread for parallel showing.
		 */
		MyThread _parallelShower;

		/**
		 * \brief Static method for parallel updating interface.
		 * \param mt Pointer to mutex for locking thread.
		 * \param flag Pointer to flag for ending this thread.
		 * \param instance Pointer to instance of this class.
		 */
		static void parallelShower(std::mutex* mt, bool* flag, MyShower* instance);

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
		 * \param newField Pointer to new field.
		 * \return Id of this link in storage.
		 */
		int addField(Message* newField);

		/**
		 * \brief Method for adding new log.
		 * \param str Description of this log.
		 */
		void addLog(std::string str);

		/**
		 * \brief Method for adding new log.
		 * \tparam T Type of information.
		 * \param str Description of this log.
		 * \param obj Object for showing.
		 */
		template<typename T>
		void addLog(std::string str, T obj);

		/**
		 * \brief Method for deleting all logs.
		 */
		void clearLog();

		/**
		 * \brief Static method for getting instance of this class.
		 * \return Instance of this class.
		 */
		static MyShower& getInstance();

		/**
		 * \brief Method for deleting field by id.
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

#include "MyShowerDefinition.inl"

#endif