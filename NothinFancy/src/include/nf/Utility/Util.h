#pragma once
#include <string>

#include "nf/Utility/log.h"
#include "nf/Utility/timing.h"
#include "nf/Utility/file.h"
#include "nf/Utility/compression.h"

#define NFSleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

namespace nf::util {
	std::string toStr(const std::wstring& in);
	std::wstring toWideStr(const std::string& in);

	double getRand();
	double getRandRange(double minimum, double maximum);

	uint64_t getMemoryUsage();
	void printMemoryUsage();
}