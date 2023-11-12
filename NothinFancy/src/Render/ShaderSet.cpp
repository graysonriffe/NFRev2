#include "pch.h"
#include "ShaderSet.h"

namespace nf::render {
	ShaderSet::ShaderSet(ComPtr<ID3D11Device> device, const std::string& vsBytecode, const std::string& psBytecode) {
		device->CreateVertexShader(vsBytecode.c_str(), vsBytecode.size(), nullptr, m_vs.GetAddressOf());
		device->CreatePixelShader(psBytecode.c_str(), psBytecode.size(), nullptr, m_ps.GetAddressOf());
	}

	void ShaderSet::bind(ComPtr<ID3D11DeviceContext> context) {
		context->VSSetShader(m_vs.Get(), nullptr, 0);
		context->PSSetShader(m_ps.Get(), nullptr, 0);
	}

	ShaderSet::~ShaderSet() {

	}
}