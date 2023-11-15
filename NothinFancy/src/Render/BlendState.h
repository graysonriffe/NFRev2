#pragma once
#include <d3d11_3.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace nf::render {
	class BlendState {
	public:
		BlendState(ComPtr<ID3D11Device> device);

		void bind(ComPtr<ID3D11DeviceContext> context);

		~BlendState();
	private:
		ComPtr<ID3D11BlendState> m_state;
	};
}