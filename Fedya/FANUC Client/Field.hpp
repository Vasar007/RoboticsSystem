#ifndef MY_FIELD
#define MY_FIELD
#pragma once

#include "Message.hpp"
#include <iostream>
#include <Windows.h>

/**
* \brief Namespace of thread protected conosole inteface.
*/
namespace myInterface
{
	/**
	 * \brief Class for showing field with changable information.
	 * \tparam T Type of this information.
	 */
	template<typename T>
	class Field: public Message
	{
		/**
		 * \brief Object which is now showing.
		 */
		T _object;

	public:
		
		/**
		 * \brief Getter of object.
		 * \return Object.
		 */
		T getObject();

		/**
		 * \brief Setter of object.
		 * \param newObject Object ofr showing.
		 */
		void setObject(T newObject);

		/**
		 * \brief Constructor.
		 * \param discription Discription of this field.
		 * \param object Object for showing.
		 */
		Field(std::string discription, T object);

		/**
		 * \brief Destructor.
		 */
		virtual ~Field();
	};

	
};

#include "FieldDifinition.inl"

#endif