#include "nf.h"

int main(int argc, char* argv[]) {

	{
		nf::Config conf;
		conf.appName = "Example App";
		nf::Application app(conf);
		app.run();
	}

	return 0;
}