#include "pch.h"
#include "nf/Asset/AssetBuild.h"

#include "nf/Utility/Util.h"

namespace nf::asset {
	bool buildAssets(std::filesystem::path assetDir) {
		NFLog(std::format("Asset Builder - {}", "Test"));

		return true;
	}
}