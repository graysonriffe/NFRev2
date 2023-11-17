#pragma once
#include <vector>

#include <d3d11_3.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace nf::render {
	class Framebuffer {
	public:
		Framebuffer(ComPtr<ID3D11Device> device, unsigned int count = 1);

		void clear(ComPtr<ID3D11DeviceContext> context, const float color[4]);
		void bind(ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11Device> device, unsigned int scWidth, unsigned int scHeight);
		ComPtr<ID3D11RenderTargetView> getRTV(unsigned int index = 0);

		~Framebuffer();
	private:
		void recreate(ComPtr<ID3D11Device> device, unsigned int scWidth, unsigned int scHeight);

		const unsigned int m_count;
		std::vector<ComPtr<ID3D11RenderTargetView>> m_rtvs;
		ComPtr<ID3D11DepthStencilView> m_depthView;
		unsigned int m_width, m_height;
	};
}