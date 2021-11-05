#include "config.hpp"

#define CONFIG_VALUE(type, name) ::ConfigManager::Types::type* name = ::ConfigManager::add<::ConfigManager::Types::type>(#name);
namespace Config {
	DECLARE_CONFIG_VALUES
}
#undef CONFIG_VALUE