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

	}

	Model::~Model() {

	}
}