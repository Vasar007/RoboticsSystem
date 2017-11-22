#ifndef STATIC_FIELD_DIFINITION
#define STATIC_FIELD_DIFINITION
#pragma once

#include "MyShower.hpp"

template <typename T>
myInterface::StaticField<T>::StaticField(std::string str, T object)
:Field<T>(str,object)
{
	_pos = MyShower::getInstance().addField(this);
}

template <typename T>
myInterface::StaticField<T>::~StaticField()
{
	MyShower::getInstance().deleteField(_pos);
}

#endif