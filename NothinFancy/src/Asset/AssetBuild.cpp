#include "pch.h"
#include "nf/Asset/AssetBuild.h"

#define NFASSETBUILDLOG
#include "nf/Utility/Util.h"

#define Error(x, y) NFLog(std::format("Build of {} failed! {}", x, y))

namespace nf::asset {
	static bool writePack(const std::string& filename, std::string& contents, COMPRESSOR_HANDLE comp);

	bool buildAssets(std::filesystem::path assetDir) {
		NFLog("Startup");

		std::string packExtention = ".nfpack";

		std::set<std::string> extentions;
		extentions.insert({ "cso", "obj", "png", "jpg" });

		COMPRESSOR_HANDLE compressor;
		CreateCompressor(COMPRESS_ALGORITHM_XPRESS_HUFF, NULL, &compressor);

		unsigned int dirCount = 0;

		for (const auto& currRoot : std::filesystem::directory_iterator(assetDir)) {
			if (!currRoot.is_directory()) continue;

			dirCount++;
			std::string currPackName = currRoot.path().filename().string().append(packExtention);
			NFLog(std::format("Building pack \"{}\"", currPackName));

			std::vector<std::pair<std::string, size_t>> currPackFiles;
			std::string currFileIn, currPackOut;

			for (const auto& currFile : std::filesystem::recursive_directory_iterator(currRoot)) {
				if (currFile.is_directory()) continue;

				std::string currFilename = currFile.path().filename().string(), currFileExtension = currFile.path().extension().string().substr(1);
				NFLog(std::format("Current file: \"{}\"", currFilename));

				if (extentions.find(currFileExtension) == extentions.end()) {
					Error(currPackName, std::format("\"{}\" is not of supported file type!", currFilename));
					return false;
				}

				for (const auto& existingFile : currPackFiles)
					if (currFilename == existingFile.first) {
						Error(currPackName, std::format("Duplicate asset \"{}\" in pack!", currFilename));
						return false;
					}

				if (!util::readFile(currFile.path().string(), currFileIn)) {
					Error(currPackName, std::format("Could not read asset \"{}\"!", currFilename));
					return false;
				}

				currPackFiles.push_back({ currFilename, currFileIn.size() });

				currPackOut.append(currFileIn);
			}

			std::string currPackHeader;
			currPackHeader.append(std::format("{}\n", currPackFiles.size()));
			for (const auto& file : currPackFiles)
				currPackHeader.append(std::format("{}\n{}\n", file.first, file.second));

			currPackOut.insert(0, currPackHeader);

			NFLog(std::format("Writing \"{}\"...", currPackName));
			if (!writePack(currPackName, currPackOut, compressor)) {
				Error(currPackName, "Could not write the pack!");
				return false;
			}

			NFLog(std::format("Wrote \"{}\" containing {} file(s)", currPackName, currPackFiles.size()));
		}

		CloseCompressor(compressor);

		if (dirCount)
			NFLog(std::format("Built {} asset pack(s)", dirCount));
		else
			NFLog("No directories found!");

		NFLog(std::format("Done"));
		return true;
	}

	bool writePack(const std::string& filename, std::string& contents, COMPRESSOR_HANDLE comp) {
		NFLog("Obfuscating...");
		std::for_each(contents.begin(), contents.end(), [](char& c) { c += 100; });

		NFLog("Compressing...");
		std::string out;
		size_t compSize = 0, temp;
		Compress(comp, contents.data(), contents.size(), NULL, NULL, &compSize);
		out.resize(compSize);
		Compress(comp, contents.data(), contents.size(), out.data(), compSize, &temp);

		if (!util::writeFile(filename, out))
			return false;

		return true;
	}
}