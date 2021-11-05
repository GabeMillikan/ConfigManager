#pragma once

#define DECLARE_CONFIG_VALUES \
	CONFIG_VALUE(Color, titleBarBackground) \
	CONFIG_VALUE(Float, FOV) \
	CONFIG_VALUE(String, username)

namespace ConfigManager {
	constexpr const char* defaultConfig = 
		"titleBarBackground:#3264C8FF\n"
		"FOV:100\n"
		"username:GabeMillikan\n"
	;
}