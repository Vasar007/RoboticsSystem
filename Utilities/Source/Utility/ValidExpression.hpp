#ifndef VALID_EXPRESSION_HPP
#define VALID_EXPRESSION_HPP

#include <type_traits>


namespace utils
{

// Not-so-important helpers.
namespace detail
{

    template <template <class...> class E, class C, class... T>
    struct ValidExpressionT : public std::false_type
    {
    };

    template <template <class...> class E, class... T>
    struct ValidExpressionT<E, std::void_t<E<T...> >, T...> : public std::true_type
    {
    };

} // namespace detail

template <class T, class O>
constexpr decltype(auto) templatize(O&& value) noexcept
{
    return std::forward<O>(value);
}

// The final templated boolean.
template <template <class...> class Expr, class... Args>
constexpr auto VALID_EXPRESSION = detail::ValidExpressionT<Expr, void, Args...>::value;

} // namespace utils

#endif // VALID_EXPRESSION_HPP
