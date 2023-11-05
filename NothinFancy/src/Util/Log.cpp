#include "pch.h"
#include "nf/Log.h"

#include "nf/Util.h"

namespace nf::util {
	static std::mutex s_logMutex;

	static const char* getTypeStr(LogType type) {
		switch (type) {
			case EngLog:
				return "Engine Log";
			case UserLog:
				return "Log";
			case EngError:
				return "Engine Error";
			case UserError:
				return "Error";
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
			case UserLog:
				color = 14;
				break;
			case EngError:
				color = 4;
				break;
			case UserError:
				color = 4;
				break;
		}

		SetConsoleTextAttribute(console, color);
		std::cout << getTypeStr(type);
		SetConsoleTextAttribute(console, 7);
		std::cout << "]: ";
	}

	void logImpl(const std::string& str, LogType type) {
		std::scoped_lock logLock(s_logMutex);
		printHeader(type);
		std::cout << std::format("{}\n", str);
	}

	void errImplD(const char* str, LogType type, const char* file, unsigned int line) {
		std::scoped_lock logLock(s_logMutex);
		printHeader(type);
		std::cout << std::format("({}, {}) {}\n", file, line, str);
		MessageBeep(0);
		__debugbreak();
	}

	void errImplRel(const char* str, LogType type, const char* file, unsigned int line) {
		std::string out = std::format("{}: {}\n\nAt: {}, Line {}", getTypeStr(type), str, file, line);
		MessageBox(nullptr, toWideStr(out).c_str(), L"Nothin' Fancy Error", MB_OK | MB_ICONERROR);
		std::exit(1);
	}
}