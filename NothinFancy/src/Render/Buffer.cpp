#include "pch.h"
#include "Buffer.h"

namespace nf::render {
	using enum Buffer::Type;

	Buffer::Buffer(ComPtr<ID3D11Device> device, Type type, const void* data, size_t size, size_t stride)
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

			case Constant:
				bind = D3D11_BIND_CONSTANT_BUFFER;
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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
				context->IASetVertexBuffers(0, 1, m_buffer.GetAddressOf(), reinterpret_cast<const unsigned int*>(&m_stride), &offset);
				break;
			}

			case Index:
				context->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				break;

			case Constant:
				context->VSSetConstantBuffers(0, 1, m_buffer.GetAddressOf());
				break;
		}
	}

	void Buffer::update(ComPtr<ID3D11DeviceContext> context, const void* data, size_t size) {
		D3D11_MAPPED_SUBRESOURCE mapped = {};
		context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapped);
		std::memcpy(mapped.pData, data, size);
		context->Unmap(m_buffer.Get(), 0);
	}

	Buffer::~Buffer() {

	}
}