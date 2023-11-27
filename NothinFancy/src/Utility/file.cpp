#include "pch.h"
#include "nf/Utility/file.h"

namespace nf::util {
	bool readFile(const std::string& filename, std::string& out) {
		std::ifstream stream(filename, std::ios::binary | std::ios::ate);
		if (!stream.is_open())
			return false;

		size_t fileSize = stream.tellg();
		out.resize(fileSize);
		stream.seekg(0);
		stream.read(out.data(), fileSize);

		stream.close();
		return true;
	}

	bool writeFile(const std::string& filename, const std::string& in) {
		std::ofstream stream(filename, std::ios::binary | std::ios::trunc);
		if (!stream.is_open())
			return false;

		stream.write(in.c_str(), in.size());

		stream.close();
		return true;
	}

	bool readConfig(const std::string& filename, ConfigFile& out) {
		std::string confStr;

		if (!readFile(filename, confStr))
			return false;

		std::stringstream ss(confStr);

		out.clear();
		std::string line, key, value;
		while (std::getline(ss, line)) {
			if (line.empty())
				continue;
			std::stringstream ssLine(line);
			ssLine >> key;
			if (ssLine.eof())
				return false;
			ssLine >> value;
			if (!ssLine.eof())
				return false;

			out[key] = value;
		}

		return true;
	}

	bool writeConfig(const std::string& filename, const ConfigFile& data) {
		std::string out;

		for (auto& kv : data)
			out += std::format("{} {}\n", kv.first, kv.second);

		//out.erase(out.end() - 1);

		return writeFile(filename, out);
	}
}