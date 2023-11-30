#include "pch.h"
#include "nf/Asset/AssetBuild.h"

#define NFASSETBUILDLOG
#include "nf/Utility/Util.h"
#include "Vector.h"

#define Error(x) NFLog(std::format("Build of {} failed!", x))

namespace nf::asset {
	using ExtensionSet = std::set<std::string>;
	using Files = std::unordered_map<std::string, std::string>;

	static bool buildPack(const std::filesystem::directory_entry root, std::string packName);
	static bool loadFiles(ExtensionSet& extensions, Files& files, const std::filesystem::directory_entry root);
	static bool cookModels(Files& models, Files& textures);
	static void cookModelMaterials(std::string& material, std::vector<std::string>& order, Files& textures);
	static std::string getEndOfLine(std::stringstream& ss);
	static bool cookModel(std::string& model, std::vector<std::string>& materialOrder);
	static bool writePack(const std::string& filename, std::string& contents);

	void buildAssets(std::filesystem::path assetDir) {
		NFLog("Startup");

		std::string packExtension = ".nfpack";

		unsigned int packCount = 0;

		for (const auto& currRoot : std::filesystem::directory_iterator(assetDir)) {
			if (!currRoot.is_directory()) continue;

			std::string currPackName = currRoot.path().filename().string() + packExtension;
			NFLog(std::format("Building pack \"{}\"", currPackName));

			if (buildPack(currRoot, currPackName))
				packCount++;
		}

		if (packCount)
			NFLog(std::format("Built {} asset pack(s)", packCount));
		else
			NFLog("No packs built!");

		NFLog(std::format("Done"));
	}

	bool buildPack(const std::filesystem::directory_entry root, std::string packName) {
		Files textures;
		ExtensionSet textureExtensions({ "png", "jpg" });
		if (!loadFiles(textureExtensions, textures, root)) {
			Error(packName);
			return false;
		}

		Files models;
		ExtensionSet modelExtensions({ "obj", "mtl" });
		if (!loadFiles(modelExtensions, models, root)) {
			Error(packName);
			return false;
		}

		//Cook models while removing used textures from textures
		if (!cookModels(models, textures)) {
			Error(packName);
			return false;
		}

		//Load everything else
		Files assets;

		//Merge everything and write
		assets.merge(textures);
		assets.merge(models);

		if (assets.empty()) {
			NFLog("No assets found!");
			Error(packName);
			return false;
		}

		std::string currPackOut;

		std::string currPackHeader;
		currPackHeader += std::format("{}\n", assets.size());
		for (const auto& file : assets)
			currPackHeader += std::format("{}\n{}\n", file.first, file.second.size());

		currPackOut.insert(0, currPackHeader);

		for (const auto& file : assets)
			currPackOut += file.second;

		NFLog(std::format("Writing \"{}\"...", packName));
		if (!writePack(packName, currPackOut)) {
			Error(packName);
			return false;
		}

		NFLog(std::format("Wrote \"{}\" containing {} asset(s)", packName, assets.size()));
		return true;
	}

	bool loadFiles(ExtensionSet& extensions, Files& files, const std::filesystem::directory_entry root) {
		files.clear();

		for (const auto& currFile : std::filesystem::recursive_directory_iterator(root)) {
			if (currFile.is_directory()) continue;

			std::string currFilename = currFile.path().filename().string(), currFileExtension = currFile.path().extension().string().substr(1);
			if (extensions.find(currFileExtension) == extensions.end())
				continue;

			NFLog(std::format("Current file: \"{}\"", currFilename));

			for (const auto& existingFile : files)
				if (currFilename == existingFile.first) {
					NFLog(std::format("Duplicate asset \"{}\" in pack!", currFilename));
					return false;
				}

			std::string& currFileIn = files[currFilename];

			if (!util::readFile(currFile.path().string(), currFileIn)) {
				NFLog(std::format("Could not read asset \"{}\"!", currFilename));
				return false;
			}
		}

		return true;
	}

	bool cookModels(Files& models, Files& textures) {
		std::string materialExtension = "mtl";

		for (auto& model : models) {
			std::string extension = model.first.substr(model.first.find_last_of('.') + 1);
			if (extension == materialExtension)
				continue;

			std::string modelName = model.first.substr(0, model.first.find_last_of('.') + 1);
			std::string materialName = modelName + materialExtension;

			std::string& material = models[materialName];

			std::vector<std::string> materialOrder;
			cookModelMaterials(material, materialOrder, textures);

			if (materialOrder.empty()) {
				NFLog(std::format("Model \"{}\" has no materials!", model.first));
				return false;
			}

			if (!cookModel(model.second, materialOrder))
				return false;

			model.second.insert(0, material);

			models.erase(materialName);
		}

		return true;
	}

	void cookModelMaterials(std::string& material, std::vector<std::string>& order, Files& textures) {
		std::string mat = material;
		material.clear();
		order.clear();

		struct MaterialWrite {
			Vec3 diffuseColor;
			std::string diffuseTexture;
			std::string specularTexture;
			std::string normalTexture;
		};
		std::vector<MaterialWrite> mats;

		std::stringstream ss(mat);

		std::string line;
		while (std::getline(ss, line)) {
			std::stringstream ssLine(line);
			std::string op;
			ssLine >> op;

			if (op == "newmtl") {
				order.push_back(getEndOfLine(ssLine));
				mats.emplace_back();
			}
			else if (op == "Kd") {
				float r = 0.0f, g = 0.0f, b = 0.0f;
				ssLine >> r >> g >> b;
				mats.back().diffuseColor = Vec3(r, g, b);
			}
			else if (op == "map_Kd") {
				std::string textureName = getEndOfLine(ssLine);
				mats.back().diffuseTexture = textures[textureName];
				textures.erase(textureName);
			}
			else if (op == "map_Ks") {
				std::string textureName = getEndOfLine(ssLine);
				mats.back().specularTexture = textures[textureName];
				textures.erase(textureName);
			}
			else if (op == "map_Bump") {
				//Do I always need to do this?
				std::string temp;
				ssLine >> temp >> temp;

				std::string textureName = getEndOfLine(ssLine);
				mats.back().normalTexture = textures[textureName];
				textures.erase(textureName);
			}
		}

		material += std::format("{}\n", mats.size());
		for (const auto& mat : mats) {
			material += std::format("{} {} {}\n", mat.diffuseColor.x, mat.diffuseColor.y, mat.diffuseColor.z);

			bool hasDiffuseTex = !mat.diffuseTexture.empty();
			material += std::format("{}\n", hasDiffuseTex);
			if (hasDiffuseTex)
				material += mat.diffuseTexture;

			bool hasSpecularTex = !mat.specularTexture.empty();
			material += std::format("{}\n", hasSpecularTex);
			if (hasSpecularTex)
				material += mat.specularTexture;

			bool hasNormalTex = !mat.normalTexture.empty();
			material += std::format("{}\n", hasNormalTex);
			if (hasNormalTex)
				material += mat.normalTexture;
		}
	}

	std::string getEndOfLine(std::stringstream& ss) {
		std::string out;
		std::getline(ss, out);
		return out.substr(1);
	}

	bool cookModel(std::string& model, std::vector<std::string>& materialOrder) {
		std::stringstream ss(model);
		model.clear();

		std::vector<Vec3> rawV;

		bool hasVT = false;
		std::vector<Vec2> rawTC;

		bool hasVN = false;
		std::vector<Vec3> rawVN;

		std::vector<Vec3> rawTan;

		struct VInd {
			unsigned int v, t, n;
		};
		std::vector<std::vector<VInd>> rawInds;
		rawInds.resize(materialOrder.size());

		unsigned int currentMaterial = 0;

		std::string line;
		while (std::getline(ss, line)) {
			std::stringstream ssLine(line);
			std::string op;
			ssLine >> op;

			if (op == "v") {
				float x = 0.0f, y = 0.0f, z = 0.0f;
				ssLine >> x >> y >> z;
				rawV.push_back({ x, y, -z });
			}
			else if (op == "vn") {
				hasVN = true;
				float x = 0.0f, y = 0.0f, z = 0.0f;
				ssLine >> x >> y >> z;
				rawVN.push_back({ x, y, -z });
			}
			else if (op == "vt") {
				hasVT = true;
				float u = 0.0f, v = 0.0f;
				ssLine >> u >> v;
				rawTC.push_back({ u, 1.0f - v });
			}
			else if (op == "usemtl") {
				std::string materialName = getEndOfLine(ssLine);
				for (int i = 0; i < materialOrder.size(); i++)
					if (materialOrder[i] == materialName)
						currentMaterial = i;
			}
			else if (op == "f") {
				if (!hasVN) {
					NFLog("Model has no normals!");
					return false;
				}
				if (!hasVT) {
					NFLog("Model has no texture coordinates!");
					return false;
				}

				unsigned int v1 = 0, t1 = 0, n1 = 0, v2 = 0, t2 = 0, n2 = 0, v3 = 0, t3 = 0, n3 = 0;
				char temp = 0;
				ssLine >> v1 >> temp >> t1 >> temp >> n1 >> v2 >> temp >> t2 >> temp >> n2 >> v3 >> temp >> t3 >> temp >> n3;
				rawInds[currentMaterial].push_back({ v1, t1, n1 });
				rawInds[currentMaterial].push_back({ v3, t3, n3 });
				rawInds[currentMaterial].push_back({ v2, t2, n2 });

				Vec3 pos1 = rawV[v1 - 1];
				Vec3 pos2 = rawV[v3 - 1];
				Vec3 pos3 = rawV[v2 - 1];
				Vec2 tc1 = rawTC[t1 - 1];
				Vec2 tc2 = rawTC[t3 - 1];
				Vec2 tc3 = rawTC[t2 - 1];

				Vec3 edge1 = pos2 - pos1;
				Vec3 edge2 = pos3 - pos1;
				Vec2 delta1 = tc2 - tc1;
				Vec2 delta2 = tc3 - tc1;

				float f = 1.0f / (delta1.x * delta2.y - delta2.x * delta1.y);
				Vec3 tan;
				tan.x = f * (delta2.y * edge1.x - delta1.y * edge2.x);
				tan.y = f * (delta2.y * edge1.y - delta1.y * edge2.y);
				tan.z = f * (delta2.y * edge1.z - delta1.y * edge2.z);

				rawTan.push_back(tan);
				rawTan.push_back(tan);
				rawTan.push_back(tan);
			}
		}

		struct Vertex {
			Vec3 pos;
			Vec2 tc;
			Vec3 norm;
			Vec3 tan;

			bool operator<(const Vertex other) const {
				return std::memcmp(this, &other, sizeof(Vertex)) > 0;
			}
		};

		std::vector<Vertex> vertexBuffer;
		std::vector<std::vector<unsigned int>> indexBuffers;
		indexBuffers.resize(materialOrder.size());

		std::map<Vertex, unsigned int> vMap;
		unsigned int currIndex = 0, tanPos = 0;
		for (int i = 0; i < rawInds.size(); i++) {
			const auto& currMatRawInds = rawInds[i];
			for (int j = 0; j < currMatRawInds.size(); j++) {
				VInd currInd = currMatRawInds[j];
				Vertex currVertex = { rawV[currInd.v - 1], rawTC[currInd.t - 1], rawVN[currInd.n - 1], rawTan[tanPos + j] };

				bool found = vMap.find(currVertex) != vMap.end();
				if (found) {
					indexBuffers[i].push_back(vMap[currVertex]);
					continue;
				}

				vertexBuffer.push_back(currVertex);
				indexBuffers[i].push_back(currIndex);
				vMap[currVertex] = currIndex;
				currIndex++;
			}

			tanPos += static_cast<unsigned int>(currMatRawInds.size());
		}

		size_t vertexBufferSize = vertexBuffer.size() * sizeof(Vertex);
		model += std::format("{}\n", vertexBufferSize);
		model.append(reinterpret_cast<char*>(vertexBuffer.data()), vertexBufferSize);

		for (auto& indexBuffer : indexBuffers) {
			size_t indexBufferSize = indexBuffer.size() * sizeof(unsigned int);
			model += std::format("{}\n", indexBufferSize);
			model.append(reinterpret_cast<char*>(indexBuffer.data()), indexBufferSize);
		}

		return true;
	}

	bool writePack(const std::string& filename, std::string& contents) {
		NFLog("Obfuscating...");
		std::for_each(contents.begin(), contents.end(), [](char& c) { c += 100; });

		NFLog("Compressing...");
		std::string out;
		if (!nf::util::compress(contents, out)) {
			NFLog("Could not compress pack!");
			return false;
		}

		if (!util::writeFile(filename, out)) {
			NFLog("Could not write pack!");
			return false;
		}

		return true;
	}
}