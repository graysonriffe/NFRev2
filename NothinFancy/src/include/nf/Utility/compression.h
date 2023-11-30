#pragma once
#include <string>

namespace nf::util {
	bool compress(std::string& in, std::string& out);
	bool decompress(std::string& in, std::string& out);
}