#pragma once
#include <d3d11_3.h>
#include <wrl/client.h>

#include "Window.h"

#include "Buffer.h"

using Microsoft::WRL::ComPtr;

template <typename T>
using UP = std::unique_ptr<T>;

namespace nf::render {
	class Renderer {
	public:
		Renderer(Window& window, DisplayConfig& dispConfig);

		void setDisplay(DisplayConfig& conf);
		void doFrame();

		~Renderer();
	private:
		Window& m_window;

		ComPtr<ID3D11Device3> m_device;
		ComPtr<ID3D11DeviceContext4> m_context;
		ComPtr<IDXGISwapChain2> m_sc;
		ComPtr<ID3D11RenderTargetView1> m_outRTV;

		//Test stuff
		ComPtr<ID3D11VertexShader> m_testVertexShader;
		ComPtr<ID3D11PixelShader> m_testPixelShader;
		UP<Buffer> m_testBuffer;
		ComPtr<ID3D11InputLayout> m_testLayout;
	};
}