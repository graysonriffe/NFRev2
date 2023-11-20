#pragma once
#include <d3d11_3.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace nf::render {
	class Buffer {
	public:
		enum class Type {
			Vertex,
			Index,
			Constant
		};

		Buffer(ComPtr<ID3D11Device> device, Type type, const void* data, size_t size, size_t stride = 0);

		void bind(ComPtr<ID3D11DeviceContext> context);
		void update(ComPtr<ID3D11DeviceContext> context, const void* data, size_t size);
		ComPtr<ID3D11Buffer> getBuffer() const;

		~Buffer();
	private:
		const Type m_type;
		ComPtr<ID3D11Buffer> m_buffer;
		const size_t m_stride;
	};
}