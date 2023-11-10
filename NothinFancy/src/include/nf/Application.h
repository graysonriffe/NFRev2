#pragma once
#include <future>

#include "nf/Config.h"

namespace nf {
	class Window;

	class Application {
	public:
		Application(Config config);

		void run();
		void quit();

		~Application();
	private:
		void runInputThread(std::promise<Window&> promiseWindow);
		void handleWindowEvents(Window& window);

		Config m_config;

		bool m_running;
	};
}