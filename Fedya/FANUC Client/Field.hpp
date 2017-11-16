#ifndef MY_FIELD
#define MY_FIELD
#pragma once

#include "Message.hpp"
#include <iostream>
#include <Windows.h>

namespace myInterface
{
	template<typename T>
	class Field: public Message
	{
		T _object;

	public:
		
		T getObject();

		void setObject(T newObject);

		Field(std::string discription, T object);

		virtual ~Field();
	};

	
};

#include "FieldDifinition.inl"

#endif