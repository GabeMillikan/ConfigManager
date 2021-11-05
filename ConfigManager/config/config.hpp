#pragma once
#include "config_manager.hpp"
#include "config_values.hpp"

#define CONFIG_VALUE(type, name) extern ::ConfigManager::Types::type* name;
namespace Config {
	DECLARE_CONFIG_VALUES
}
#undef CONFIG_VALUE