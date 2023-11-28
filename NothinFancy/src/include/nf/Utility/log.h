#pragma once

#ifdef _DEBUG

#ifdef NFENGINE

#ifdef NFASSETBUILDLOG
#define NFLog(x) ::nf::util::logImpl(x, ::nf::util::LogType::AssetBuild)
#else
#define NFLog(x) ::nf::util::logImpl(x, ::nf::util::LogType::EngLog)
#endif

#define NFError(x) ::nf::util::errImplD(x, ::nf::util::LogType::EngError, std::strrchr(__FILE__, '\\') + 1, __LINE__)

#else
#define NFLog(x) ::nf::util::logImpl(x, ::nf::util::LogType::Log)
#define NFError(x) ::nf::util::errImplD(x, ::nf::util::LogType::Error, std::strrchr(__FILE__, '\\') + 1, __LINE__)
#endif

#else //Release

#ifdef NFASSETBUILDLOG
#define NFLog(x) ::nf::util::logImpl(x, ::nf::util::LogType::AssetBuild)
#else
#define NFLog(x)
#endif

#ifdef NFENGINE
#define NFError(x) ::nf::util::errImplRel(x, ::nf::util::LogType::EngError, std::strrchr(__FILE__, '\\') + 1, __LINE__)
#else
#define NFError(x) ::nf::util::errImplRel(x, ::nf::util::LogType::Error, std::strrchr(__FILE__, '\\') + 1, __LINE__)
#endif

#endif

namespace nf::util {
	enum class LogType {
		EngLog,
		Log,
		EngError,
		Error,
		Timing,
		AssetBuild
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