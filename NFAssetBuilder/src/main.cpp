#include "nf.h"

int main(int argc, char* argv[]) {
	std::filesystem::path workingDir = std::filesystem::current_path();

	bool result = nf::asset::buildAssets(workingDir);

	return result ? 0 : 1;
}