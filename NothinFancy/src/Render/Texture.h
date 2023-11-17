#pragma once
#include <string>

#include <d3d11_3.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace nf::render {
	class Texture {
	public:
		Texture(ComPtr<ID3D11Device> device, const std::string& data);
		Texture(ComPtr<ID3D11Device> device, unsigned int width, unsigned int height, bool depth = false);

		void bind(ComPtr<ID3D11DeviceContext> context);
		ComPtr<ID3D11Texture2D> getTexture() const;

		~Texture();
	private:
		ComPtr<ID3D11Texture2D> m_texture;
		ComPtr<ID3D11ShaderResourceView> m_view;
	};
}