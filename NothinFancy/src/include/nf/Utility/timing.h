#pragma once
#include <chrono>

#ifdef _DEBUG

#define NFTime() ::nf::util::Timer __funcTimer(__FUNCSIG__)

#else

#define NFTime

#endif

namespace nf::util {
	class Timer {
	public:
		Timer(const char* funcSig);

		~Timer();
	private:
		const char* m_func;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
	};
}