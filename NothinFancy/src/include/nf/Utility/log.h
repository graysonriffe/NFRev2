#pragma once

#ifdef _DEBUG

#ifdef NFENGINE

#define NFLog(x) ::nf::util::logImpl(x, ::nf::util::LogType::EngLog);
#define NFError(x) ::nf::util::errImplD(x, ::nf::util::LogType::EngError, std::strrchr(__FILE__, '\\') + 1, __LINE__);

#else

#define NFLog(x) ::nf::util::logImpl(x, ::nf::util::LogType::UserLog);
#define NFError(x) ::nf::util::errImplD(x, ::nf::util::LogType::UserError, std::strrchr(__FILE__, '\\') + 1, __LINE__);

#endif

#else

#define NFLog

#ifdef NFENGINE

#define NFError(x) ::nf::util::errImplRel(x, ::nf::util::LogType::EngError, std::strrchr(__FILE__, '\\') + 1, __LINE__)

#else

#define NFError(x) ::nf::util::errImplRel(x, ::nf::util::LogType::UserError, std::strrchr(__FILE__, '\\') + 1, __LINE__)

#endif

#endif

namespace nf::util {
	enum class LogType {
		EngLog,
		UserLog,
		EngError,
		UserError,
		Timing
	};

	void logImpl(const char* str, LogType type);
	void logImpl(const std::string& str, LogType type);

	template<typename T>
	void logImpl(T in, LogType type) {
		logImpl(std::to_string(in), type);
	}

	void errImplD(const std::string& str, LogType type, const char* file, unsigned int line);
	void errImplRel(const std::string& str, LogType type, const char* file, unsigned int line);
}