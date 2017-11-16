#ifndef MY_MESSAGE
#define MY_MESSAGE
#pragma once

#include <string>
#include <Windows.h>

/**
 * \brief Namespace of thread protected conosole inteface.
 */
namespace myInterface 
{
	/**
	 * \brief Class of filed showing.
	 */
	class Message
	{
	protected:

		/**
		* \brief Discription of this message.
		*/
		std::string _discription;

		/**
		 * \brief Informstoin in this message.
		 */
		std::string _message;

		/**
		 * \brief Flag if this message should be updateed.
		 */
		bool _needUpdate = true;

	public:

		/**
		 * \brief Default constructor.
		 */
		Message() = default;

		/**
		 * \brief Constructor for showing number.
		 * \param discription String with discription of this message.
		 * \param message String with Information in this message.
		 */
		explicit Message(std::string discription, std::string message);

		/**
		 * \brief Method for "quick" ouputting this message.
		 */
		void showQuick(HANDLE hConsole,int line);

		/**
		 * \brief Method for outputting this message.
		 */
		void show();

		virtual ~Message() = default;
	};

	
}

#include "MessageDifinition.inl"

#endif