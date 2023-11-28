#include "pch.h"
#include "nf/Utility/log.h"

#include "nf/Utility/Util.h"

namespace nf::util {
	static std::mutex s_logMutex;

	using enum LogType;

	static const char* getTypeStr(LogType type) {
		switch (type) {
			case EngLog:
				return "Engine Log";
			case Log:
				return "Log";
			case EngError:
				return "Engine Error";
			case Error:
				return "Error";
			case Timing:
				return "Timing";
			case AssetBuild:
				return "Asset Builder";
			default:
				return "Unknown";
		}
	}

	static void printHeader(LogType type) {
		static HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

		std::cout << "[NF ";

		WORD color = 7;
		switch (type) {
			case EngLog:
				color = 6;
				break;
			case Log:
				color = 14;
				break;
			case EngError:
				color = 4;
				break;
			case Error:
				color = 4;
				break;
			case Timing:
				color = 10;
				break;
			case AssetBuild:
				color = 3;
				break;
		}

		SetConsoleTextAttribute(console, color);
		std::cout << getTypeStr(type);
		SetConsoleTextAttribute(console, 7);
		std::cout << "]: ";
	}

	void logImpl(const char* str, LogType type) {
		std::scoped_lock logLock(s_logMutex);
		printHeader(type);
		std::cout << std::format("{}\n", str);
	}

	void logImpl(const std::string& str, LogType type) {
		logImpl(str.c_str(), type);
	}

	void errImplD(const std::string& str, LogType type, const char* file, unsigned int line) {
		std::scoped_lock logLock(s_logMutex);
		printHeader(type);
		std::cout << std::format("({}, {}) {}\n", file, line, str);
		MessageBeep(0);
		__debugbreak();
	}

	void errImplRel(const std::string& str, LogType type, const char* file, unsigned int line) {
		std::string out = std::format("{}: {}\n\nAt: {}, Line {}", getTypeStr(type), str, file, line);
		MessageBox(nullptr, toWideStr(out).c_str(), L"Nothin' Fancy Error", MB_OK | MB_ICONERROR);
		std::exit(1);
	}
}