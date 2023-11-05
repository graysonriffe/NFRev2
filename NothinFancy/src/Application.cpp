#include "pch.h"
#include "nf/Application.h"

#include "nf/Util.h"

namespace nf {
	Application::Application() {

	}

	void Application::run() {
#ifdef _DEBUG
		SetThreadDescription(GetCurrentThread(), L"Main Thread");
		std::string consoleTitle = std::format("Nothin' Fancy Debug Console (" NFVERSION ") - {}\n", "App Name");
		SetConsoleTitle(util::toWideStr(consoleTitle).c_str());
#endif

		NFLog("Nothin' Fancy " NFVERSION);

		std::string test = "String Test";
		NFLog(test);

		NFLog("Literal Test");

		NFError("Something broke!");

		std::cin.get();
	}

	Application::~Application() {

	}
}