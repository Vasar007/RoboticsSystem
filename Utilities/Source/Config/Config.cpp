#include "Config.h"


namespace config
{

NamedConfig::NamedConfig(
    const std::initializer_list<std::map<std::string, std::any>::value_type> init)
    : _storage(init)
{
}

} // namespace config
