#include <iostream>

#include "config/config.hpp"
namespace C = Config;
namespace CM = ConfigManager;

int main()
{
	CM::loads(CM::defaultConfig);
	
	std::cout << "Title bar background: " << C::titleBarBackground->toString() << std::endl;

	C::titleBarBackground->set(50, 100, 200);

	std::cout << "Full configuration: \n" << CM::dumps() << std::endl;
}