#pragma once
#include <d3d11_3.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace nf::render {
	class SamplerState {
	public:
		SamplerState(ComPtr<ID3D11Device> device);

		void bind(ComPtr<ID3D11DeviceContext> context);

		~SamplerState();
	private:
		ComPtr<ID3D11SamplerState> m_state;
	};
}