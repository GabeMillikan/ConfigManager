#pragma once
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <fstream>

namespace TextService {
	constexpr const char* whitespace = " \t\n\r\f\v";

	inline std::string& rtrim(std::string& s)
	{
		s.erase(s.find_last_not_of(whitespace) + 1);
		return s;
	}

	inline std::string& ltrim(std::string& s)
	{
		s.erase(0, s.find_first_not_of(whitespace));
		return s;
	}

	inline std::string& trim(std::string& s)
	{
		return ltrim(rtrim(s));
	}

	inline std::string hex(unsigned char byte)
	{
		// is this a good solution? no
		// is it efficient? yes
		constexpr const char* lookup =
			"000102030405060708090A0B0C0D0E0F"
			"101112131415161718191A1B1C1D1E1F"
			"202122232425262728292A2B2C2D2E2F"
			"303132333435363738393A3B3C3D3E3F"
			"404142434445464748494A4B4C4D4E4F"
			"505152535455565758595A5B5C5D5E5F"
			"606162636465666768696A6B6C6D6E6F"
			"707172737475767778797A7B7C7D7E7F"
			"808182838485868788898A8B8C8D8E8F"
			"909192939495969798999A9B9C9D9E9F"
			"A0A1A2A3A4A5A6A7A8A9AAABACADAEAF"
			"B0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF"
			"C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF"
			"D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF"
			"E0E1E2E3E4E5E6E7E8E9EAEBECEDEEEF"
			"F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF"
		;

		return std::string(lookup + (byte << 1), 2);
	}

	inline unsigned char fromHexStr(char c1, char c2)
	{
		unsigned char ret = 0;

		if ('0' <= c1 && c1 <= '9') ret = (c1 - '0') << 4;
		else ret = (c1 - 'A') << 4;

		if ('0' <= c2 && c2 <= '9') ret = c2 - '0';
		else ret += c2 - 'A';

		return ret;
	}
}

namespace ConfigManager
{
	namespace Types {
		struct Base {
			virtual std::string toString() = 0;
			virtual bool fromString(const std::string&) = 0;
		};

		struct Color : public Base
		{
			unsigned char rgba[4];

			Color() = default;
			Color(const unsigned char& r, const unsigned char& g, const unsigned char& b, const unsigned char& a = 255);
			Color(const float& r, const float& g, const float& b, const float& a = 1.f);

			constexpr unsigned char& r();
			constexpr unsigned char& g();
			constexpr unsigned char& b();
			constexpr unsigned char& a();
			void set(const unsigned char& r, const unsigned char& g, const unsigned char& b, const unsigned char& a = 255);

			std::string toString();
			bool fromString(const std::string& s);
		};

		struct Integer : public Base
		{
			int value;

			Integer() = default;
			Integer(const float& value);
			Integer(const double& value);
			Integer(const int& value);

			std::string toString();
			bool fromString(const std::string& s);
		};

		struct Float : public Base
		{
			float value;

			Float() = default;
			Float(const float& value);
			Float(const double& value);
			Float(const int& value);

			std::string toString();
			bool fromString(const std::string& s);
		};

		struct String : public Base
		{
			std::string value;

			String() = default;
			String(const char* value);
			String(const std::string& value);

			std::string toString();
			bool fromString(const std::string& s);
		};
	}

	std::map<std::string, std::shared_ptr<Types::Base>>& getTable();

	template <typename T>
	T* add(const std::string& name)
	{
		T* pValue = new T();
		auto upValue = std::shared_ptr<Types::Base>(pValue);
		getTable().insert(std::make_pair(name, upValue));
		return pValue;
	}

	size_t loads(const std::string& config, const bool& loadAll = true, const std::vector<std::string>& loadThese = {});
	std::string dumps(const bool& dumpAll = true, const std::vector<std::string>& dumpThese = {});

	size_t load(std::fstream& config, const bool& loadAll = true, const std::vector<std::string>& loadThese = {});
	void dump(std::fstream& outFile, const bool& dumpAll = true, const std::vector<std::string>& dumpThese = {});
}