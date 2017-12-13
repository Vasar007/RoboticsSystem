#ifndef FIELD_H
#define FIELD_H

#include "Message.hpp"

/**
* \brief Namespace of thread protected console interface.
*/
namespace myInterface
{

/**
 * \brief       Class for showing field with changeable information.
 * \tparam T    Type of this information.
 */
template<typename T>
class Field: public Message
{
private:

    /**
	 * \brief Object which is now showing.
	 */
	T _object;

public:
		
	/**
	 * \brief   Object getter.
	 * \return  Object.
	 */
	T getObject();

    /**
	 * \brief       Object setter.
	 * \param[in]   newObject Object for showing.
	 */
	void setObject(const T& newObject);

    /**
	 * \brief       Constructor.
	 * \param[in]   description Description of this field.
	 * \param[in]   object Object for showing.
	 */
	explicit Field(const std::string& description,const T& object);

	/**
	 * \brief Destructor.
	 */
	virtual ~Field() = default;
};
	
}

#include "Field.inl"

#endif // FIELD_H