#include "pch.h"
#include "nf/Utility/random.h"

#undef max

namespace nf::util {
	double getRand() {
		static std::random_device s_dev;

		return static_cast<double>(s_dev()) / s_dev.max();
	}

	double getRandRange(double minimum, double maximum) {
		return getRand() * (maximum - minimum) + minimum;
	}
}