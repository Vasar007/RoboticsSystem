#ifndef MY_SHOWER_H
#define MY_SHOWER_H

#include <map>
#include <mutex>
#include <vector>


#include "StaticField.hpp"
#include "MyThread.hpp"

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
private:
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
	 * \brief               Static method for parallel updating interface.
	 * \param[in] mt        Link to mutex for locking thread.
	 * \param[in] flag      Pointer to flag for ending this thread.
	 * \param[in] instance  Pointer to instance of this class.
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
	 * \brief               Method for adding new field.
	 * \param[in] newField  Pointer to new field.
	 * \return              Id of this link in storage.
	 */
	int addField(Message* newField);

	/**
	 * \brief                       Method for adding new log.
	 * \param[in] newDescription    Description of this log.
	 */
	void addLog(const std::string& newDescription);

	/**
	 * \brief           Method for adding new log.
	 * \tparam T        Type of information.
	 * \param[in] str   Description of this log.
	 * \param[in] obj   Object for showing.
	 */
	template<typename T>
	void addLog(const std::string& str,const T& obj);

	/**
	 * \brief Method for deleting all logs.
	 */
	void clearLog();

	/**
	 * \brief   Static method for getting instance of this class.
	 * \return  Instance of this class.
	 */
	static MyShower& getInstance();

	/**
	 * \brief   Method for deleting field by id.
	 * \param   fieldId Id of field.
	 */
	void deleteField(int fieldId);

    /**
	 * \brief Destructor.
	 */
	~MyShower();
};

}

/*ToDo
 * Case of adding and removing too many fields.
 * Reverse logs.
 * Adding controlling for supervisor.
 */

#include "MyShower.inl"

#endif // MY_SHOWER_H