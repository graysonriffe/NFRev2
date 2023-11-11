#pragma once

namespace nf {
	struct DisplayConfig {
		enum class Mode {
			Windowed,
			BorderlessFullscreen
		};

		unsigned int width, height;
		Mode mode;

		DisplayConfig()
			: width(1280)
			, height(720)
			, mode(Mode::Windowed)
		{}
	};

	struct Config {
		const char* appName;
		const char* appVersion;
		DisplayConfig display;

		Config()
			: appName("Nothin' Fancy Application")
			, appVersion("0.1.0")
			, display()
		{}
	};
}