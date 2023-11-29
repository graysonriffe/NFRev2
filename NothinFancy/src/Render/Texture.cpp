#include "pch.h"
#include "Texture.h"

#include "nf/Utility/Util.h"

namespace nf::render {
	Texture::Texture(ComPtr<ID3D11Device> device, const std::string& data, Type type) {
		using enum Type;

		D3D11_TEXTURE2D_DESC desc = {};
		DXGI_FORMAT texFormat = type == SRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Format = texFormat;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA sub = {};

		/*desc.Width = width;
		desc.Height = height;
		sub.pSysMem = stbData;
		sub.SysMemPitch = width * requiredChannels;*/

		device->CreateTexture2D(&desc, &sub, m_texture.GetAddressOf());


		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
		viewDesc.Format = texFormat;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipLevels = 1;

		device->CreateShaderResourceView(m_texture.Get(), &viewDesc, m_view.GetAddressOf());
	}

	Texture::Texture(ComPtr<ID3D11Device> device, unsigned int width, unsigned int height, bool depth) {
		D3D11_TEXTURE2D_DESC desc = {};
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.Width = width;
		desc.Height = height;

		if (depth) {
			desc.Format = DXGI_FORMAT_D32_FLOAT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		}
		else {
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		}

		device->CreateTexture2D(&desc, nullptr, m_texture.GetAddressOf());
	}

	void Texture::bind(ComPtr<ID3D11DeviceContext> context) {
		context->PSSetShaderResources(0, 1, m_view.GetAddressOf());
	}

	ComPtr<ID3D11Texture2D> Texture::getTexture() const {
		return m_texture;
	}

	ComPtr<ID3D11ShaderResourceView> Texture::getView() const {
		return m_view;
	}


	Texture::~Texture() {

	}
}