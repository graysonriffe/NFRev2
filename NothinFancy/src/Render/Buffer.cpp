#include "pch.h"
#include "Buffer.h"

namespace nf::render {
	using enum Buffer::Type;

	Buffer::Buffer(ComPtr<ID3D11Device> device, Type type, void* data, size_t size, unsigned int stride)
		: m_type(type)
		, m_stride(stride)
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = static_cast<unsigned int>(size);
		unsigned int bind = 0;

		switch (m_type) {
			case Vertex:
				bind = D3D11_BIND_VERTEX_BUFFER;
				break;

			case Index:
				bind = D3D11_BIND_INDEX_BUFFER;
				break;
		}
		desc.BindFlags = bind;

		D3D11_SUBRESOURCE_DATA sub = {};
		sub.pSysMem = data;

		device->CreateBuffer(&desc, &sub, m_buffer.GetAddressOf());
	}

	void Buffer::bind(ComPtr<ID3D11DeviceContext> context) {
		switch (m_type) {
			case Vertex: {
				unsigned int offset = 0;
				context->IASetVertexBuffers(0, 1, m_buffer.GetAddressOf(), &m_stride, &offset);
				break;
			}

			case Index:
				context->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				break;
		}
	}

	Buffer::~Buffer() {

	}
}