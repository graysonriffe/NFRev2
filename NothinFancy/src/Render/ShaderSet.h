#pragma once
#include <string>

#include <d3d11_3.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace nf::render {
	class ShaderSet {
	public:
		ShaderSet(ComPtr<ID3D11Device> device, const std::string& vsBytecode, const std::string& psBytecode);

		void bind(ComPtr<ID3D11DeviceContext> context);

		~ShaderSet();
	private:
		ComPtr<ID3D11VertexShader> m_vs;
		ComPtr<ID3D11PixelShader> m_ps;
	};
}