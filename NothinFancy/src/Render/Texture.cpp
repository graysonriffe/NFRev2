#include "pch.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "nf/Utility/Util.h"

namespace nf::render {
	Texture::Texture(ComPtr<ID3D11Device> device, const std::string& data) {
		D3D11_TEXTURE2D_DESC desc = {};
		DXGI_FORMAT texFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.Format = texFormat;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA sub = {};
		int width = 0, height = 0, channels = 0, requiredChannels = 4;
		unsigned char* stbData = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data.data()), static_cast<int>(data.size()), &width, &height, &channels, requiredChannels);
		if (!stbData)
			NFError("Could not load texture data!");

		desc.Width = width;
		desc.Height = height;
		sub.pSysMem = stbData;
		sub.SysMemPitch = width * requiredChannels;

		device->CreateTexture2D(&desc, &sub, m_texture.GetAddressOf());

		stbi_image_free(stbData);

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

	Texture::~Texture() {

	}
}