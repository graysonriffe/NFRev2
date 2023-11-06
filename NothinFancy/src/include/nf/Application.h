#pragma once
#include "nf/Config.h"

namespace nf {
	class Application {
	public:
		Application(Config config);

		void run();

		~Application();
	private:
		Config m_config;
	};
}