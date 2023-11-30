#pragma once
#include <string>

namespace nf::util {
	void compress(std::string& in, std::string& out);
	void decompress(std::string& in, std::string& out);
}