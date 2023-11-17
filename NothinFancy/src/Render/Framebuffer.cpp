#include "pch.h"
#include "Framebuffer.h"

#include "nf/Utility/Util.h"
#include "Texture.h"

namespace nf::render {
	Framebuffer::Framebuffer(ComPtr<ID3D11Device> device, unsigned int count)
		: m_count(count)
		, m_width(0)
		, m_height(0)
	{
		if (!count)
			NFError("Framebuffer must have at least 1 buffer!");
	}

	void Framebuffer::clear(ComPtr<ID3D11DeviceContext> context, const float color[4]) {
		for (const auto& rtv : m_rtvs)
			context->ClearRenderTargetView(rtv.Get(), color);

		if (m_depthView)
			context->ClearDepthStencilView(m_depthView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void Framebuffer::bind(ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11Device> device, unsigned int scWidth, unsigned int scHeight) {
		if (m_rtvs.empty() || !(m_width == scWidth && m_height == scHeight))
			recreate(device, scWidth, scHeight);

		std::vector<ID3D11RenderTargetView*> array;
		array.reserve(m_count);
		for (const auto& rtv : m_rtvs)
			array.emplace_back(rtv.Get());

		context->OMSetRenderTargets(m_count, array.data(), m_depthView.Get());
	}

	ComPtr<ID3D11RenderTargetView> Framebuffer::getRTV(unsigned int index) {
		return m_rtvs[index];
	}

	void Framebuffer::recreate(ComPtr<ID3D11Device> device, unsigned int scWidth, unsigned int scHeight) {
		m_width = scWidth, m_height = scHeight;

		m_rtvs.clear();
		m_depthView.Reset();

		ComPtr<ID3D11RenderTargetView> currRTV;
		for (unsigned int i = 0; i < m_count; i++) {
			Texture tex(device, m_width, m_height);

			device->CreateRenderTargetView(tex.getTexture().Get(), nullptr, currRTV.ReleaseAndGetAddressOf());
			m_rtvs.push_back(currRTV);
		}

		Texture depthTex(device, m_width, m_height, true);
		device->CreateDepthStencilView(depthTex.getTexture().Get(), nullptr, m_depthView.GetAddressOf());
	}

	Framebuffer::~Framebuffer() {

	}
}