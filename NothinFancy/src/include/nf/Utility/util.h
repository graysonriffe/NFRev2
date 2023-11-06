#pragma once
#include <string>

#include "nf/Utility/log.h"
#include "nf/Utility/timing.h"
#include "nf/Utility/random.h"

namespace nf::util {
	std::string toStr(const std::wstring& in);
	std::wstring toWideStr(const std::string& in);
}