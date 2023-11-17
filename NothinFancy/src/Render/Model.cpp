#include "pch.h"
#include "Model.h"

namespace nf::render {
	Model::Model(ComPtr<ID3D11Device> device, const std::string& objData)
		: m_indexCount(0)
	{
		std::vector<float> vBuffer;
		std::vector<unsigned int> iBuffer;
		parseOBJ(objData, vBuffer, iBuffer);

		m_vertices = std::make_unique<Buffer>(device, Buffer::Type::Vertex, vBuffer.data(), sizeof(float) * vBuffer.size(), 5 * sizeof(float));
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

		struct VPos {
			float x, y, z;
		};
		std::vector<VPos> rawV;

		struct TCoord {
			float u, v;
		};
		std::vector<TCoord> rawTC;

		struct VInd {
			unsigned int v, t;
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
				rawV.push_back({ x, y, z });
			}
			else if (op == "vt") {
				float u = 0.0f, v = 0.0f;
				ssLine >> u >> v;
				rawTC.push_back({ u, v });
			}
			else if (op == "f") {
				unsigned int v1 = 0, t1 = 0, v2 = 0, t2 = 0, v3 = 0, t3 = 0;
				char temp = 0;
				ssLine >> v1 >> temp >> t1 >> v2 >> temp >> t2 >> v3 >> temp >> t3;
				rawInd.push_back({ v1, t1 });
				rawInd.push_back({ v2, t2 });
				rawInd.push_back({ v3, t3 });
			}
		}

		struct Vertex {
			VPos pos;
			TCoord tc;

			bool operator<(const Vertex other) const {
				return std::memcmp(this, &other, sizeof(Vertex)) > 0;
			}
		};

		std::map<Vertex, unsigned int> vMap;
		unsigned int currIndex = 0;
		for (const auto& currInd : rawInd) {
			Vertex currVertex = { rawV[currInd.v - 1], rawTC[currInd.t - 1] };

			bool found = vMap.find(currVertex) != vMap.end();
			if (found) {
				indicesOut.push_back(vMap[currVertex]);
				continue;
			}

			verticesOut.push_back(currVertex.pos.x);
			verticesOut.push_back(currVertex.pos.y);
			verticesOut.push_back(currVertex.pos.z);
			verticesOut.push_back(currVertex.tc.u);
			verticesOut.push_back(currVertex.tc.v);
			indicesOut.push_back(currIndex);
			vMap[currVertex] = currIndex;
			currIndex++;
		}
	}

	Model::~Model() {

	}
}