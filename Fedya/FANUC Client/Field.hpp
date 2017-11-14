#pragma once

#ifndef MY_FIELD

#define MY_FIELD
#include <string>
#include <vector>
#include <Windows.h>

/**
 * \brief 
 */
namespace myInterface 
{
	/**
	 * \brief Class of filed showing.
	 */
	class Field
	{
		/**
		 * \brief Discription of this field.
		 */
		std::string _mes;
		
		/**
		 * \brief Pointer to int for showing
		 */
		int* _val;
		
		/**
		 * \brief Vector of strings for showing.
		 */
		std::vector<std::string> _param;

	public:

		/**
		 * \brief Default constructor.
		 */
		Field() = default;

		/**
		 * \brief Constructor for showing number.
		 * \param mes String with discription of this field.
		 * \param num Pointer to unt for showing.
		 */
		Field(std::string mes, int* num);

		/**
		 * \brief Constructor with strings for field.
		 * \param mes String with discription of field.
		 * \param num Pointer to int for sowing.
		 * \param var Vector of strings for showing.
		 */
		Field(std::string mes, int* num, std::vector<std::string> var);

		/**
		 * \brief Method for "quick" ouputting this field.
		 * \param hconsole HANDLE of console.
		 * \param line Number of this field.
		 */
		void showQuick(HANDLE hconsole, int line) const;

		/**
		 * \brief Method for outputting this field.
		 */
		void show() const;
	};

	
}

#include "FiledDifinition.hpp"

#endif