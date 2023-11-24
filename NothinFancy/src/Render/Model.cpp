#include "pch.h"
#include "Model.h"

#include "Vector.h"
#include "nf/Utility/Util.h"

namespace nf::render {
	Model::Model(ComPtr<ID3D11Device> device, const std::string& objData)
		: m_indexCount(0)
	{
		std::vector<float> vBuffer;
		std::vector<unsigned int> iBuffer;
		parseOBJ(objData, vBuffer, iBuffer);

		m_vertices = std::make_unique<Buffer>(device, Buffer::Type::Vertex, vBuffer.data(), sizeof(float) * vBuffer.size(), 11 * sizeof(float));
		m_indices = std::make_unique<Buffer>(device, Buffer::Type::Index, iBuffer.data(), sizeof(unsigned int) * iBuffer.size());

		m_indexCount = static_cast<unsigned int>(iBuffer.size());
	}

	void Model::bind(ComPtr<ID3D11DeviceContext> context) {
		m_vertices->bind(context);
		m_indices->bind(context);
	}

	unsigned int Model::getIndexCount() const {
		return m_indexCount;
	}

	void Model::parseOBJ(const std::string& objData, std::vector<float>& verticesOut, std::vector<unsigned int>& indicesOut) {
		verticesOut.clear();
		indicesOut.clear();

		std::stringstream ss(objData);

		std::vector<Vec3> rawV;

		bool hasVT = false;
		std::vector<Vec2> rawTC;

		bool hasVN = false;
		std::vector<Vec3> rawVN;

		std::vector<Vec3> rawTan;

		struct VInd {
			unsigned int v, t, n;
		};
		std::vector<VInd> rawInd;

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
			else if (op == "f") {
				if (!hasVN)
					NFError("OBJ model has no normals!");
				if (!hasVT)
					NFError("OBJ model has no texture coordinates!");

				unsigned int v1 = 0, t1 = 0, n1 = 0, v2 = 0, t2 = 0, n2 = 0, v3 = 0, t3 = 0, n3 = 0;
				char temp = 0;
				ssLine >> v1 >> temp >> t1 >> temp >> n1 >> v2 >> temp >> t2 >> temp >> n2 >> v3 >> temp >> t3 >> temp >> n3;
				rawInd.push_back({ v1, t1, n1 });
				rawInd.push_back({ v3, t3, n3 });
				rawInd.push_back({ v2, t2, n2 });

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

		std::map<Vertex, unsigned int> vMap;
		unsigned int currIndex = 0;
		for (int i = 0; i < rawInd.size(); i++) {
			VInd currInd = rawInd[i];
			Vertex currVertex = { rawV[currInd.v - 1], rawTC[currInd.t - 1], rawVN[currInd.n - 1], rawTan[i]};

			bool found = vMap.find(currVertex) != vMap.end();
			if (found) {
				indicesOut.push_back(vMap[currVertex]);
				continue;
			}

			verticesOut.push_back(currVertex.pos.x);
			verticesOut.push_back(currVertex.pos.y);
			verticesOut.push_back(currVertex.pos.z);
			verticesOut.push_back(currVertex.tc.x);
			verticesOut.push_back(currVertex.tc.y);
			verticesOut.push_back(currVertex.norm.x);
			verticesOut.push_back(currVertex.norm.y);
			verticesOut.push_back(currVertex.norm.z);
			verticesOut.push_back(currVertex.tan.x);
			verticesOut.push_back(currVertex.tan.y);
			verticesOut.push_back(currVertex.tan.z);
			indicesOut.push_back(currIndex);
			vMap[currVertex] = currIndex;
			currIndex++;
		}
	}

	Model::~Model() {

	}
}