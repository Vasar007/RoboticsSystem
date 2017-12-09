#ifndef MY_FIELD
#define MY_FIELD
#pragma once

#include "Message.hpp"

/**
* \brief Namespace of thread protected console interface.
*/
namespace myInterface
{
	/**
	 * \brief Class for showing field with changeable information.
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
		 * \brief Object getter.
		 * \return Object.
		 */
		T getObject();

		/**
		 * \brief Object setter.
		 * \param newObject Object for showing.
		 */
		void setObject(T newObject);

		/**
		 * \brief Constructor.
		 * \param description Description of this field.
		 * \param object Object for showing.
		 */
		Field(std::string description, T object);

		/**
		 * \brief Destructor.
		 */
		virtual ~Field();
	};

	
};

#include "FieldDefinition.inl"

#endif