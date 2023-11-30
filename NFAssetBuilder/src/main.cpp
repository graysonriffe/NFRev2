#include "nf.h"

int main(int argc, char* argv[]) {
	std::filesystem::path workingDir = std::filesystem::current_path();

	nf::asset::buildAssets(workingDir);

	return 0;
}