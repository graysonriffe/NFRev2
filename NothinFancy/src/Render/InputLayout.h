#pragma once
#include <string>
#include <vector>

#include <d3d11_3.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace nf::render {
	class InputLayout {
	public:
		InputLayout();

		void pushFloat(const char* semantic, unsigned int count);
		void create(ComPtr<ID3D11Device> device, const std::string& vsBytecode);
		void bind(ComPtr<ID3D11DeviceContext> context);

		~InputLayout();
	private:
		ComPtr<ID3D11InputLayout> m_layout;

		std::vector<D3D11_INPUT_ELEMENT_DESC> m_elements;
	};
}