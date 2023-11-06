#include "pch.h"
#include "nf/Application.h"

#include "version.h"
#include "nf/Utility/util.h"

namespace nf {
	Application::Application(Config config)
		: m_config(config)
	{}

	void Application::run() {
#ifdef _DEBUG
		SetThreadDescription(GetCurrentThread(), L"Main Thread");
		std::string consoleTitle = std::format("Nothin' Fancy Debug Console (" NFVERSION ") - {} {}", m_config.appName, m_config.appVersion);
		SetConsoleTitle(util::toWideStr(consoleTitle).c_str());
#endif
		NFLog(std::format("Nothin' Fancy {}", NFVERSION));
		NFLog(std::format("Starting {} {}", m_config.appName, m_config.appVersion));

		util::printMemoryUsage();

		NFLog("Shutdown");
		std::cin.get();
	}

	Application::~Application() {

	}
}