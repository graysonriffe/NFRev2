#include "pch.h"
#include "nf/Utility/timing.h"

#include "nf/Utility/log.h"

namespace nf::util {
	Timer::Timer(const char* funcSig) {
		m_func = funcSig;
		m_startTime = std::chrono::high_resolution_clock::now();
	}

	Timer::~Timer() {
		std::chrono::duration<double, std::micro> time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_startTime);

		logImpl(std::format("Scope in {} took {} us ({} ms)", m_func, time.count(), time.count() * 0.001), LogType::Timing);
	}
}