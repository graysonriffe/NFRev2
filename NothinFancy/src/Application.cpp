#include "pch.h"
#include "nf/Application.h"

#include "nf/Utility/util.h"

namespace nf {
	Application::Application() {

	}

	void Application::run() {
#ifdef _DEBUG
		SetThreadDescription(GetCurrentThread(), L"Main Thread");
		std::string consoleTitle = std::format("Nothin' Fancy Debug Console (" NFVERSION ") - {}\n", "App Name");
		SetConsoleTitle(util::toWideStr(consoleTitle).c_str());
#endif



		std::cin.get();
	}

	Application::~Application() {

	}
}