#define NFENTRY
#include "nf.h"

nf::Config nf::configApp(nf::CommandLineArguments cmd) {
	nf::Config conf;

	conf.appName = "Example App";

	return conf;
}