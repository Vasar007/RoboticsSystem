#ifndef MY_STATIC_FIELD
#define MY_STATIC_FIELD
#pragma once

#include "Field.hpp"

/**
* \brief Namespace of thread protected console interface.
*/
namespace myInterface
{
	/**
	 * \brief Class for continuously showing field with changeable information.
	 * \tparam T Type of information.
	 */
	template<typename T>
	class StaticField:public Field<T>
	{

		/**
		 * \brief Id of this field.
		 */
		int _id;

	public:
		/**
		 * \brief Constructor with adding this field to storage
		 * \param str Description of this field.
		 * \param object Object for showing.
		 */
		StaticField(std::string str, T object);

		/**
		 * \brief Destructor with erasing this field from storage.
		 */
		~StaticField();
	};

	
}

#include "StaticFieldDefinition.inl"

#endif