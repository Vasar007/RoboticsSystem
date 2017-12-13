#ifndef STATIC_FIELD_H
#define STATIC_FIELD_H

#include "Field.hpp"

/**
* \brief Namespace of thread protected console interface.
*/
namespace myInterface
{

/**
 * \brief       Class for continuously showing field with changeable information.
 * \tparam T    Type of information.
 */
template<typename T>
class StaticField:public Field<T>
{
private:

	/**
	 * \brief Id of this field.
	 */
	int _id;

public:
	
    /**
	 * \brief               Constructor with adding this field to storage
	 * \param[in] str       Description of this field.
	 * \param[in] object    Object for showing.
	 */
	explicit StaticField(const std::string& str,const T& object);

	/**
	 * \brief Destructor with erasing this field from storage.
	 */
	~StaticField();
};
	
}

#include "StaticField.inl"

#endif // STATIC_FIELD_H 