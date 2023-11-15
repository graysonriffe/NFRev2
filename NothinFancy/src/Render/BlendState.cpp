#include "pch.h"
#include "BlendState.h"

namespace nf::render {
	BlendState::BlendState(ComPtr<ID3D11Device> device) {
		D3D11_BLEND_DESC desc = {};
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		device->CreateBlendState(&desc, m_state.GetAddressOf());
	}

	void BlendState::bind(ComPtr<ID3D11DeviceContext> context) {
		context->OMSetBlendState(m_state.Get(), nullptr, 0xffffffff);
	}

	BlendState::~BlendState() {

	}
}