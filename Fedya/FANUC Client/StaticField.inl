#ifndef STATIC_FIELD_INL
#define STATIC_FIELD_INL

#include "MyShower.hpp"

template <typename T>
myInterface::StaticField<T>::StaticField(const std::string& str,const T& object)
    : Field<T>(str,object)
{
	_id = MyShower::getInstance().addField(this);
}

template <typename T>
myInterface::StaticField<T>::~StaticField()
{
	MyShower::getInstance().deleteField(_id);
}

#endif // STATIC_FIELD_INL