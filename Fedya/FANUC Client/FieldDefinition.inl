#ifndef FIELD_DIFINITION
#define FIELD_DIFINITION
#pragma once

#include <sstream>

template <typename T>
T myInterface::Field<T>::getObject()
{
	return _object;
}

template <typename T>
void myInterface::Field<T>::setObject(T newObject)
{
	if(_object != newObject)
	{
		std::stringstream  sbuf;
		sbuf << newObject;
		_message = sbuf.str();
		_object = newObject;
		_needUpdate = true;
	}
}

template <typename T>
myInterface::Field<T>::Field(std::string difinition, T object)
	: Message(difinition,"")
{
	setObject(object);
}

template <typename T>
myInterface::Field<T>::~Field()
{
}

#endif
