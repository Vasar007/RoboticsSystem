#ifndef MY_STATIC_FIELD
#define MY_STATIC_FIELD
#pragma once

#include "Field.hpp"

/**
* \brief Namespace of thread protected conosole inteface.
*/
namespace myInterface
{
	/**
	 * \brief Class for continuously showing field with changeable information.
	 * \tparam T Type of infirmation.
	 */
	template<typename T>
	class StaticField:public Field<T>
	{

		/**
		 * \brief Position of this field in storage.
		 */
		int _pos;

	public:
		/**
		 * \brief Constructor with adding this field to storrage
		 * \param str Discription of this field.
		 * \param object Object for showing.
		 */
		StaticField(std::string str, T object);

		/**
		 * \brief Distructor with erasing this field from storrage.
		 */
		~StaticField();
	};

	
}

#include "StaticFieldDifinition.inl"

#endif