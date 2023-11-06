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

		std::string first, second = "A really\nreally\ncool file!!", file = "test.txt", file2 = "write.txt";

		if (!util::readFile(file, first))
			NFError(std::format("Could not read file \"{}\"", file));

		NFLog(first);

		if (!util::writeFile("write.txt", second))
			NFError(std::format("Could not write file \"{}\"", file2));

		util::ConfigFile conf;
		conf["displayType"] = "windowed";
		conf["name"] = "player";
		conf["health"] = "100";

		if (!util::writeConfig("save1", conf))
			NFError("Could not write config file!");

		util::ConfigFile options;

		if (!util::readConfig("save1", options))
			NFError("Could not read config file!");

		for (auto& kv : options)
			NFLog(std::format("{} = {}", kv.first, kv.second));

		std::cin.get();
	}

	Application::~Application() {

	}
}