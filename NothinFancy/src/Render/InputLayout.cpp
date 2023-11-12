#include "pch.h"
#include "InputLayout.h"

#include "nf/Utility/Util.h"

namespace nf::render {
	InputLayout::InputLayout() {

	}

	void InputLayout::pushFloat(const char* semantic, unsigned int count) {
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		switch (count) {
			case 1:
				format = DXGI_FORMAT_R32_FLOAT;
				break;
			case 2:
				format = DXGI_FORMAT_R32G32_FLOAT;
				break;
			case 3:
				format = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case 4:
				format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
		}

		m_elements.push_back({ semantic, 0, format, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	}

	void InputLayout::create(ComPtr<ID3D11Device> device, const std::string& vsBytecode) {
		device->CreateInputLayout(m_elements.data(), static_cast<unsigned int>(m_elements.size()), vsBytecode.data(), vsBytecode.size(), m_layout.GetAddressOf());
	}

	void InputLayout::bind(ComPtr<ID3D11DeviceContext> context) {
		if (!m_layout)
			NFError("D3D11 InputLayout bound before creation!");

		context->IASetInputLayout(m_layout.Get());
	}

	InputLayout::~InputLayout() {

	}
}