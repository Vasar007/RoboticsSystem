#ifndef MY_STATIC_FIELD
#define MY_STATIC_FIELD
#pragma once

#include "Field.hpp"

namespace myInterface
{
	template<typename T>
	class StaticField:public Field<T>
	{

		int _pos;

	public:
		StaticField(std::string str, T object);

		~StaticField();
	};

	
}

#include "StaticFieldDifinition.inl"
#endif