#pragma once
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
		void handleWindowEvents(Window& window);

		Config m_config;

		bool m_running;
	};
}