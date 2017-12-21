#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <Windows.h>

/**
 * \brief Namespace of thread protected console interface.
 */
namespace myInterface 
{
	
/**
 * \brief Class of field showing.
 */
class Message
{
protected:

    /**
	* \brief Description of this message.
	*/
	std::string _description;
    
    /**
	 * \brief Informatoin in this message.
	 */
	std::string _message;

	/**
	 * \brief Flag if this message should be update.
	 */
	bool _needUpdate = true;

public:

	/**
	 * \brief                   Constructor.
	 * \param[in] description   String with description of this message.
	 * \param[in] message       String with information in this message.
	 */
	explicit Message(const std::string& description,const std::string& message);

    /**
     * \brief               Method for "quick" outputting this message.
	 * \param[in] hConsole  Handle of console.
	 * \param[in] line      Line for outputting.
	 */
	void showQuick(HANDLE hConsole,int line);

    /**
	 * \brief Method for outputting this message.
	 */
	void show();

	/**
	 * \brief   Method for taking length of field.
	 * \return  Size of field.
	 */
	size_t size() const;

	/**
	 * \brief Destructor.
	 */
	virtual ~Message() = default;
};

}

#endif //MESSAGE_H