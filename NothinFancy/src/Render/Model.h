#pragma once
#include <string>
#include <vector>

#include <d3d11_3.h>
#include <wrl/client.h>

#include "Buffer.h"

using Microsoft::WRL::ComPtr;

template <typename T>
using UP = std::unique_ptr<T>;

namespace nf::render {
	class Model {
	public:
		Model(ComPtr<ID3D11Device> device, const std::string& objData);

		void bind(ComPtr<ID3D11DeviceContext> context);
		unsigned int getIndexCount() const;

		~Model();
	private:
		void parseOBJ(const std::string& objData, std::vector<float>& verticesOut, std::vector<unsigned int>& indicesOut);

		UP<Buffer> m_vertices;
		UP<Buffer> m_indices;
		unsigned int m_indexCount;
	};
}