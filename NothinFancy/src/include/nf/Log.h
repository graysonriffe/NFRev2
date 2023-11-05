#pragma once

#ifdef _DEBUG

#ifdef NFENGINE

#define NFLog(x) ::nf::util::logImpl(x, ::nf::util::EngLog);
#define NFError(x) ::nf::util::errImplD(x, ::nf::util::EngError, std::strrchr(__FILE__, '\\') + 1, __LINE__);

#else

#define NFLog(x) ::nf::util::logImpl(x, ::nf::util::UserLog);
#define NFError(x) ::nf::util::errImplD(x, ::nf::util::UserError, std::strrchr(__FILE__, '\\') + 1, __LINE__);

#endif

#else

#define NFLog

#ifdef NFENGINE

#define NFError(x) ::nf::util::errImplRel(x, ::nf::util::EngError, std::strrchr(__FILE__, '\\') + 1, __LINE__)

#else

#define NFError(x) ::nf::util::errImplRel(x, ::nf::util::UserError, std::strrchr(__FILE__, '\\') + 1, __LINE__)

#endif

#endif

namespace nf::util {
	enum LogType {
		EngLog,
		UserLog,
		EngError,
		UserError
	};

	void logImpl(const std::string& str, LogType type);

	template<typename T>
	void logImpl(T in, LogType type) {
		logImpl(static_cast<std::string>(in), type);
	}

	void errImplD(const char* str, LogType type, const char* file, unsigned int line);
	void errImplRel(const char* str, LogType type, const char* file, unsigned int line);
}