#include "pch.h"
#include "nf/Utility/Util.h"

#undef max

namespace nf::util {
	std::string toStr(const std::wstring& in) {
		std::string out;
		out.resize(in.size());
		wcstombs_s(nullptr, out.data(), out.size() + 1, in.c_str(), out.size());

		return out;
	}

	std::wstring toWideStr(const std::string& in) {
		return std::wstring(in.begin(), in.end());
	}

	double getRand() {
		static std::random_device s_dev;

		return static_cast<double>(s_dev()) / s_dev.max();
	}

	double getRandRange(double minimum, double maximum) {
		return getRand() * (maximum - minimum) + minimum;
	}

	uint64_t getMemoryUsage() {
		PROCESS_MEMORY_COUNTERS_EX mem = {};
		GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&mem), sizeof(mem));

		return mem.PrivateUsage;
	}

	void printMemoryUsage() {
		uint64_t mem = getMemoryUsage();
		NFLog(std::format("Memory Usage: {} bytes ({:.1f} MiB)", mem, mem / 1048576.0));
	}
}