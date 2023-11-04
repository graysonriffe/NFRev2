#include "pch.h"
#include "nf/Application.h"

namespace nf {
	Application::Application() {

	}

	void Application::run() {
		std::cout << "Nothin' Fancy " NFVERSION "\n";

		std::cin.get();
	}

	Application::~Application() {

	}
}