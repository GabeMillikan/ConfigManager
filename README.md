# ConfigManager
Lightweight library to access and save/load configurable data.

# Demo
```cpp
#include <iostream>
#include "config/config.hpp"

int main()
{
	ConfigManager::loads(ConfigManager::defaultConfig);
	std::cout << "Title bar background: " << Config::titleBarBackground->toString() << std::endl;

	Config::titleBarBackground->set(50, 100, 200);
	std::cout << "Full configuration: \n" << ConfigManager::dumps() << std::endl;
}
```

# Usage
1. Edit the `config/config_values.hpp` file to add all of the variables.
2. Access and edit config values through the Config namespace like so `Config::MyConfigVariableName->value = 5`.
3. Save/Load
    a. To/from a string by using `ConfigManager::dumps`/`ConfigManager::loads`
    b. To/from a file by using `ConfigManager::dump`/`ConfigManager::load`
