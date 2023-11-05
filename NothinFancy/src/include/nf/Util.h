#pragma once
#include <string>

#include "nf/Log.h"

namespace nf::util {
	std::string toStr(const std::wstring& in);
	std::wstring toWideStr(const std::string& in);
}