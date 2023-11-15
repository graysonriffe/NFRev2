#include "pch.h"
#include "SamplerState.h"

namespace nf::render {
	SamplerState::SamplerState(ComPtr<ID3D11Device> device) {
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		device->CreateSamplerState(&desc, m_state.GetAddressOf());
	}

	void SamplerState::bind(ComPtr<ID3D11DeviceContext> context) {
		context->PSSetSamplers(0, 1, m_state.GetAddressOf());
	}

	SamplerState::~SamplerState() {

	}
}