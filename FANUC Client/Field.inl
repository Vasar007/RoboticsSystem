#ifndef FIELD_INL
#define FIELD_INL

#include <sstream>

template <typename T>
T myInterface::Field<T>::getObject()
{
	return _object;
}

template <typename T>
void myInterface::Field<T>::setObject(const T& newObject)
{
	if(_object != newObject)
	{
		// Converting object to string.
	    std::stringstream  stringConverter;
		stringConverter << newObject;

        // Setting new object.
		_message = stringConverter.str();
		_object = newObject;

        // Change to update-able status.
		_needUpdate = true;
	}
}

template <typename T>
myInterface::Field<T>::Field(const std::string& difinition,const T& object)
	: Message(difinition,"")
{
	setObject(object);
}

#endif // FIELD_INL
