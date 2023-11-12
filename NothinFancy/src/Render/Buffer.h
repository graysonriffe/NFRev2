#pragma once
#include <d3d11_3.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace nf::render {
	class Buffer {
	public:
		enum class Type {
			Vertex,
			Index
		};

		Buffer(ComPtr<ID3D11Device> device, Type type, void* data, size_t size, unsigned int stride);

		void bind(ComPtr<ID3D11DeviceContext> context);

		~Buffer();
	private:
		const Type m_type;
		ComPtr<ID3D11Buffer> m_buffer;
		const unsigned int m_stride;
	};
}