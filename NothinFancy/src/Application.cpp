#include "nf/Application.h"

#include <iostream>

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