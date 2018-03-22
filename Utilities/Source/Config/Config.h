#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>
#include <any>
#include <cassert>
#include <initializer_list>
#include <iostream>


/**
 * \brief Additional namespace to process lots of config parameters and constants in classes.
 */
namespace config
{

/**
 * \brief More complex config with heap allocation (std::map) but it is easier to use.
 */
struct NamedConfig
{
    /**
     * \brief       Constructor which created dictionary from arguments.
     * \param init  Config parameters in pairs (name : value) which need to keep in storage.
     */
    NamedConfig(const std::initializer_list<std::map<std::string, std::any>::value_type> init);

    /**
     * \brief           Get element with needed name from the storage.
     * \tparam T        Type of the needed element.
     * \param paramName Name of the needed element.
     * \return          Needed element.
     */
    template <class T>
    T get(const std::string_view paramName) const;

private:
    /**
     * \brief Storage of all parameters in config.
     */
    std::map<std::string, std::any, std::less<> > _storage;
};

#include "Config.inl"

}  // namespace config

#endif // CONFIG_H
