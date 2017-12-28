#ifndef PRINT_HPP
#define PRINT_HPP

#include <type_traits>
#include <iostream>
#include <string>

#include "../Utility/ValidExpression.hpp"


/*
 * Use cases:
 *
 * namespace NA
 * {
 *
 * class A
 * {
 * };
 *
 * std::string to_string(const A&)
 * {
 *   return { "to_string(A)" };
 * }
 *
 * } // namespace NA
 *
 * namespace NB
 * {
 *
 * class B
 * {
 * };
 *
 * std::ostream& operator <<(std::ostream& out, const B&)
 * {
 *   out << "oss(B)";
 *   return out;
 * }
 *
 * } // namespace NB
 *
 * namespace NC
 * {
 *
 * class C
 * {
 * public:
 *   friend std::ostream& operator <<(std::ostream& out, const C&)
 *   {
 *     out << "oss(C)";
 *     return out;
 *   }
 * };
 *
 * } // namespace NC
 *
 * namespace ND
 * {
 *
 * class D
 * {
 * public:
 *   std::string toString() const
 *   {
 *     return { "member::toString(D)" };
 *   }
 * };
 *
 * } // namespace ND
 *
 * print("Some text\n");
 * print(42);
 * print(NA::A());
 * print(NB::B());
 * print(NC::C());
 * print(ND::D());
 */

namespace utils
{

// Expressions to test validity wrapped into tyedefs using decltype.
template <typename T>
using Ostream   = decltype(std::cout << std::declval<T>());
template <typename T>
using StdFree   = decltype(std::cout << std::to_string(std::declval<T>()));
template <typename T>
using Free1      = decltype(std::cout << to_string(std::declval<T>()));
template <typename T>
using Free2 = decltype(std::cout << toString(std::declval<T>()));
template <typename T>
using Member1 = decltype(std::cout << std::declval<T>().to_string());
template <typename T>
using Member2    = decltype(std::cout << std::declval<T>().toString());

////////////////////////////// The implementations. //////////////////////////////

/**
 * \brief		        Print datatype to outstream.
 * \tparam OutputStream	Type of output stream.
 * \tparam T	        Datatype to print.
 * \param[out] out      Output stream to write.
 * \param[in] t         Data to print.
 */
template <class OutputStream, typename T>
void print(OutputStream& out, const T& t) noexcept
{
    if constexpr (VALID_EXPRESSION<Ostream, T>)
    {
        out << t;
    }
    else if constexpr(VALID_EXPRESSION<StdFree, T>)
    {
        out << std::to_string(t);
    }
    else if constexpr(VALID_EXPRESSION<Free1, T>)
    {
        out << to_string(t);
    }
    else if constexpr(VALID_EXPRESSION<Free2, T>)
    {
        out << toString(t);
    }
    else if constexpr(VALID_EXPRESSION<Member1, T>)
    {
        out << t.to_string();
    }
    else if constexpr(VALID_EXPRESSION<Member2, T>)
    {
        out << t.toString();
    }
    else
    {
        static_assert(templatize<T>(false), "T is not printable!");
    }
}

/**
 * \brief			    Print datatypes (unlimited) to outstream.
 * \tparam OutputStream	Type of output stream.
 * \tparam T		    Datatype to print.
 * \tparam Args		    Variadic datatypes.
 * \param[out] out      Output stream to write.
 * \param[in] t		    Data to print.
 * \param[in] args	    Data to print.
 */
template <class OutputStream, typename T, typename... Args>
void print(OutputStream& out, const T& t, const Args&... args) noexcept
{
    if constexpr (VALID_EXPRESSION<Ostream, T>)
    {
        out << t << ' ';
    }
    else if constexpr(VALID_EXPRESSION<StdFree, T>)
    {
        out << std::to_string(t) << ' ';
    }
    else if constexpr(VALID_EXPRESSION<Free1, T>)
    {
        out << to_string(t) << ' ';
    }
    else if constexpr(VALID_EXPRESSION<Free2, T>)
    {
        out << toString(t) << ' ';
    }
    else if constexpr(VALID_EXPRESSION<Member1, T>)
    {
        out << t.to_string() << ' ';
    }
    else if constexpr(VALID_EXPRESSION<Member2, T>)
    {
        out << t.toString() << ' ';
    }
    else
    {
        static_assert(templatize<T>(false), "T is not printable!");
    }

    print(out, args...);
}

/**
 * \brief		        Print datatype to outstream.
 * \tparam OutputStream	Type of output stream.
 * \tparam T	        Datatype to print.
 * \param[out] out      Output stream to write.
 * \param[in] t         Data to print.
 */
template <class OutputStream, typename T>
void println(OutputStream& out, const T& t) noexcept
{
    if constexpr (VALID_EXPRESSION<Ostream, T>)
    {
        out << t << '\n';
    }
    else if constexpr(VALID_EXPRESSION<StdFree, T>)
    {
        out << std::to_string(t) << '\n';
    }
    else if constexpr(VALID_EXPRESSION<Free1, T>)
    {
        out << to_string(t) << '\n';
    }
    else if constexpr(VALID_EXPRESSION<Free2, T>)
    {
        out << toString(t) << '\n';
    }
    else if constexpr(VALID_EXPRESSION<Member1, T>)
    {
        out << t.to_string() << '\n';
    }
    else if constexpr(VALID_EXPRESSION<Member2, T>)
    {
        out << t.toString() << '\n';
    }
    else
    {
        static_assert(templatize<T>(false), "T is not printable!");
    }
}

/**
 * \brief			    FPrint datatypes (unlimited) to outstream.
 * \tparam OutputStream	Type of output stream.
 * \tparam T		    Datatype to print.
 * \tparam Args		    Variadic datatypes.
 * \param[out] out      Output stream to write.
 * \param[in] t		    Data to print.
 * \param[in] args	    Data to print.
 */
template <class OutputStream, typename T, typename... Args>
void println(OutputStream& out, const T& t, const Args&... args) noexcept
{
    if constexpr (VALID_EXPRESSION<Ostream, T>)
    {
        out << t << ' ';
    }
    else if constexpr(VALID_EXPRESSION<StdFree, T>)
    {
        out << std::to_string(t) << ' ';
    }
    else if constexpr(VALID_EXPRESSION<Free1, T>)
    {
        out << to_string(t) << ' ';
    }
    else if constexpr(VALID_EXPRESSION<Free2, T>)
    {
        out << toString(t) << ' ';
    }
    else if constexpr(VALID_EXPRESSION<Member1, T>)
    {
        out << t.to_string() << ' ';
    }
    else if constexpr(VALID_EXPRESSION<Member2, T>)
    {
        out << t.toString() << ' ';
    }
    else
    {
        static_assert(templatize<T>(false), "T is not printable!");
    }

    println(out, args...);
}

} // namespace utils

#endif // PRINT_HPP
