#pragma once
#include <unordered_map>

namespace nf::util {
	bool readFile(const std::string& filename, std::string& out);
	bool writeFile(const std::string& filename, const std::string& in);

	using ConfigFile = std::unordered_map<std::string, std::string>;

	bool readConfig(const std::string& filename, ConfigFile& out);
	bool writeConfig(const std::string& filename, const ConfigFile& config);
}