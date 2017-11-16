#ifndef STATIC_FIELD_DIFINITION
#define STATIC_FIELD_DIFINITION
#pragma once

template <typename T>
myInterface::StaticField<T>::StaticField(std::string str, T object):Field<T>(str,object)
{
	_pos = MyShower::getInstance().addLine(*this);
}

template <typename T>
myInterface::StaticField<T>::~StaticField()
{
	MyShower::getInstance().deleteLine(_pos);
}

#endif