#pragma once

namespace nf {
	struct Config {
		const char* appName;
		const char* appVersion;

		Config()
			: appName("Nothin' Fancy Application")
			, appVersion("0.1.0")
		{}
	};
}