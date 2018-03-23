#ifndef CONFIG_H
#define CONFIG_H

#include <tuple>


#define CAST(x) static_cast<std::size_t>(x)

/**
 * \brief Additional namespace to process lots of config parameters and constants in classes.
 */
namespace config
{

/**
 * \brief       Class provides parameters storage with almost zero-cost overhead.
 * \tparam Args Config parameters types.
 */
template <class... Args>
struct Config
{
    /**
     * \brief    Type alias for tuple elements.
     * \tparam I Index of tuple element.
     */
    template <std::size_t I>
    using TupleElement = std::tuple_element_t<I, std::tuple<Args...> >;


    /**
     * \brief      Constructor which created tuple from arguments.
     * \param args Config parameters which need to keep in storage.
     */
    constexpr Config(Args&&... args)
        : _storage(std::make_tuple(args...))
    {
    }

    /**
     * \brief        Get element with needed index from the storage.
     * \tparam Index Index of needed element.
     * \return       Needed element.
     */
    template <std::size_t Index>
    constexpr TupleElement<Index> get() const noexcept
    {
        static_assert(Index < sizeof...(Args), "Index out of range!");
        return std::get<Index>(_storage);
    }

    /**
     * \brief Keep number of arguments in config at runtime. Alternative: std::tuple_size_v.
     */
    const std::size_t   kNumberOfParams = sizeof...(Args);


private:
    /**
     * \brief Storage of all parameters in config.
     */
    std::tuple<Args...> _storage;
};

}  // namespace config

#endif // CONFIG_H
