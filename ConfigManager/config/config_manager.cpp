#include <sstream>

#include "config_manager.hpp"

std::map<std::string, std::shared_ptr<ConfigManager::Types::Base>>& ConfigManager::getTable()
{
	static std::map<std::string, std::shared_ptr<Types::Base>> t;
	return t;
}

/*
	TYPES
*/
ConfigManager::Types::Color::Color(const unsigned char& r, const unsigned char& g, const unsigned char& b, const unsigned char& a)
{
	this->set(r, g, b, a);
}
ConfigManager::Types::Color::Color(const float& r, const float& g, const float& b, const float& a) : Color(
	(unsigned char)(r * 255.f + 0.5f),
	(unsigned char)(g * 255.f + 0.5f),
	(unsigned char)(b * 255.f + 0.5f),
	(unsigned char)(a * 255.f + 0.5f)
) {}
constexpr unsigned char& ConfigManager::Types::Color::r() { return this->rgba[0]; }
constexpr unsigned char& ConfigManager::Types::Color::g() { return this->rgba[1]; }
constexpr unsigned char& ConfigManager::Types::Color::b() { return this->rgba[2]; }
constexpr unsigned char& ConfigManager::Types::Color::a() { return this->rgba[3]; }
void ConfigManager::Types::Color::set(const unsigned char& r, const unsigned char& g, const unsigned char& b, const unsigned char& a)
{
	this->rgba[0] = r;
	this->rgba[1] = g;
	this->rgba[2] = b;
	this->rgba[3] = a;
}

std::string ConfigManager::Types::Color::toString()
{
	return "#" + TextService::hex(this->r()) + TextService::hex(this->g()) + TextService::hex(this->b()) + TextService::hex(this->a());
}

bool ConfigManager::Types::Color::fromString(const std::string& s)
{
	if (s.length() != 1 + 2 + 2 + 2 + 2) return false;
	if (s.at(0) != '#') return false;
	for (size_t o = 1; o < 9; o++)
	{
		const unsigned char c = s.at(o);
		if ('0' <= c && c <= '9' || 'A' <= c && c <= 'F') continue;
		return false;
	}

	this->r() = TextService::fromHexStr(s.at(1), s.at(2));
	this->g() = TextService::fromHexStr(s.at(3), s.at(4));
	this->b() = TextService::fromHexStr(s.at(5), s.at(6));
	this->a() = TextService::fromHexStr(s.at(7), s.at(8));

	return true;
}

ConfigManager::Types::Integer::Integer(const int& value) : value(value) {}
ConfigManager::Types::Integer::Integer(const float& value) : Integer((int)(value + .5f)) {}
ConfigManager::Types::Integer::Integer(const double& value) : Integer((int)(value + .5)) {}
std::string ConfigManager::Types::Integer::toString() {
	return std::to_string(this->value);
}
bool ConfigManager::Types::Integer::fromString(const std::string& s) {
	try {
		this->value = std::stoi(s);
	}
	catch (std::exception&) {
		return false;
	}
	return true;
}

ConfigManager::Types::Float::Float(const float& value) : value(value) {}
ConfigManager::Types::Float::Float(const double& value) : Float((float)value) {}
ConfigManager::Types::Float::Float(const int& value) : Float((float)value) {}
std::string ConfigManager::Types::Float::toString() {
	return std::to_string(this->value);
}
bool ConfigManager::Types::Float::fromString(const std::string& s) {
	try {
		this->value = std::stof(s);
	}
	catch (std::exception&) {
		return false;
	}
	return true;
}

ConfigManager::Types::String::String(const char* value) : value(value) {}
ConfigManager::Types::String::String(const std::string& value) : value(value) {}
std::string ConfigManager::Types::String::toString() {
	// replace `\` with `\\` and newlines with `\n`
	// spaces escaped as `\s` to prevent trimming
	std::string out;
	out.reserve(this->value.size());

	// config saving is only done upon user interaction,
	// so this doesn't have to be super efficient
	for (size_t i = 0; i < this->value.size(); i++)
	{
		if (this->value.at(i) == '\\') out += "\\\\";
		else if (this->value.at(i) == '\n') out += "\\n";
		else if (this->value.at(i) == ' ') out += "\\s";
		else out += this->value.substr(i, 1);
	}

	return out;
}
bool ConfigManager::Types::String::fromString(const std::string& in) {
	// see toString comments
	std::string out;

	for (size_t i = 0; i < in.size(); i++)
	{
		if (in.at(i) == '\\')
		{
			if (i + 1 >= in.size()) return false; // cannot have escape as last char
			else if (in.at(i + 1) == 'n') out += "\n";
			else if (in.at(i + 1) == '\\') out += "\\";
			else if (in.at(i + 1) == 's') out += " ";
			else return false;

			i++; // skip the n or /
		}
		else
		{
			out += in.substr(i, 1);
		}
	}

	this->value = out;

	return true;
}


/*
	SAVING/LOADING
*/
inline bool parse(const std::string& line, const bool& includeAll, const std::vector<std::string>& onlyInclude)
{
	size_t colon = line.find_first_of(':');
	if (colon == std::string::npos) return false;

	std::string name = line.substr(0, colon);
	name = TextService::trim(name);

	bool included = includeAll;
	for (size_t i = 0; !included && i < onlyInclude.size(); i++)
		included = onlyInclude[i] == name;
	if (!included) return false;

	if (ConfigManager::getTable().find(name) == ConfigManager::getTable().end()) return false;
	ConfigManager::Types::Base* prop = ConfigManager::getTable().find(name)->second.get();

	std::string value = line.substr(colon + 1, line.find_first_of('\n', colon));
	value = TextService::trim(value);

	return prop->fromString(value);
}

inline std::string dump(const std::string& name, ConfigManager::Types::Base* value, const bool& includeAll, const std::vector<std::string>& onlyInclude)
{
	bool included = includeAll;
	for (size_t i = 0; !included && i < onlyInclude.size(); i++)
		included = onlyInclude[i] == name;
	if (!included) return "";

	return name + ":" + value->toString() + "\n";
}


size_t ConfigManager::loads(const std::string& config, const bool& loadAll, const std::vector<std::string>& loadThese)
{
	std::istringstream iss(config);
	size_t amountLoaded = 0;

	for (std::string line; std::getline(iss, line); )
	{
		if (parse(line, loadAll, loadThese)) amountLoaded++;
	}
	
	return amountLoaded;
}

size_t ConfigManager::load(std::fstream& config, const bool& loadAll, const std::vector<std::string>& loadThese)
{
	size_t amountLoaded = 0;
	for (std::string line; std::getline(config, line); )
	{
		if (parse(line, loadAll, loadThese)) amountLoaded++;
	}
	return amountLoaded;
}

std::string ConfigManager::dumps(const bool& dumpAll, const std::vector<std::string>& dumpThese)
{
	std::string out;

	for (auto const& kv : ConfigManager::getTable())
	{
		const std::string& name = kv.first;
		Types::Base* value = kv.second.get();

		out += ::dump(name, value, dumpAll, dumpThese);
	}

	return out;
}

void ConfigManager::dump(std::fstream& outFile, const bool& dumpAll, const std::vector<std::string>& dumpThese)
{
	for (auto const& kv : ConfigManager::getTable())
	{
		const std::string& name = kv.first;
		Types::Base* value = kv.second.get();

		const auto& out = ::dump(name, value, dumpAll, dumpThese);
		outFile.write(out.c_str(), out.length());
	}
}

