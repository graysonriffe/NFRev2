#include "pch.h"
#include "nf/Utility/util.h"

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
}