#pragma once
//Master header for Nothin' Fancy

#include "nf/Application.h"
#include "nf/Utility/util.h"

#ifdef NFENTRY

namespace nf {
	struct CommandLineArguments {
		int argc;
		char** argv;
	};

	Config configApp(CommandLineArguments cmd);
}

int main(int argc, char* argv[]) {

	nf::Config config = nf::configApp({ argc, argv });

	{
		nf::Application app(config);
		app.run();
	}

	return 0;
}

#endif