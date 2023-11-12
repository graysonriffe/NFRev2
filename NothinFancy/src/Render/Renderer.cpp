#include "pch.h"
#include "Renderer.h"

#include "nf/Utility/Util.h"

namespace nf::render {
	static const float s_black[] = {
		0.0f, 0.0f, 0.0f, 1.0f
	};

	Renderer::Renderer(Window& window, DisplayConfig& dispConfig)
		: m_window(window)
	{
		NFLog("Initializing renderer...");

		UINT deviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_1, selectedLevel;

		ComPtr<ID3D11Device> tempDev;
		ComPtr<ID3D11DeviceContext> tempCon;
		HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, &level, 1, D3D11_SDK_VERSION, tempDev.GetAddressOf(), &selectedLevel, tempCon.GetAddressOf());
		if (hr != S_OK || selectedLevel != level)
			NFError("Could not initialize Direct3D 11! Your graphics card may not support Direct3D 11.");

		tempDev.As(&m_device);
		tempCon.As(&m_context);

		ComPtr<IDXGIAdapter2> adapter;
		{
			ComPtr<IDXGIDevice3> tempDXGIDevice;
			m_device.As(&tempDXGIDevice);
			ComPtr<IDXGIAdapter> tempAdapter;
			tempDXGIDevice->GetAdapter(&tempAdapter);
			tempAdapter.As(&adapter);
		}

		DXGI_ADAPTER_DESC2 cardDesc = {};
		adapter->GetDesc2(&cardDesc);
		NFLog(std::format("Using device \"{}\"", util::toStr(cardDesc.Description)));

		ComPtr<IDXGIFactory3> dxgiFactory;
		adapter->GetParent(__uuidof(IDXGIFactory3), reinterpret_cast<void**>(dxgiFactory.GetAddressOf()));

		DXGI_SWAP_CHAIN_DESC1 scDesc = {};
		scDesc.BufferCount = 2;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scDesc.SampleDesc.Count = 1;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		//scDesc.Flags =

		HWND hwnd = m_window.getHandle();
		ComPtr<IDXGISwapChain1> tempSC;
		hr = dxgiFactory->CreateSwapChainForHwnd(m_device.Get(), hwnd, &scDesc, nullptr, nullptr, tempSC.GetAddressOf());
		if (hr != S_OK)
			NFError("Could fully initialize Direct3D 11! Your graphics card may not support Direct3D 11.");

		tempSC.As(&m_sc);

		dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES);

		setDisplay(dispConfig);

		float triangle[] = {
			-0.5f, -0.5f,
			0.0f, 0.5f,
			0.5f, -0.5f
		};

		D3D11_BUFFER_DESC buffDesc = {};
		buffDesc.ByteWidth = sizeof(triangle);
		buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA sub = {};
		sub.pSysMem = triangle;

		m_device->CreateBuffer(&buffDesc, &sub, m_testBuffer.GetAddressOf());

		unsigned int stride = 8, offset = 0;
		m_context->IASetVertexBuffers(0, 1, m_testBuffer.GetAddressOf(), &stride, &offset);

		std::string vertexShader, pixelShader;
		util::readFile("BasicVertex.cso", vertexShader);
		util::readFile("BasicPixel.cso", pixelShader);

		m_device->CreateVertexShader(vertexShader.c_str(), vertexShader.size(), nullptr, m_testVertexShader.GetAddressOf());
		m_context->VSSetShader(m_testVertexShader.Get(), nullptr, 0);
		m_device->CreatePixelShader(pixelShader.c_str(), pixelShader.size(), nullptr, m_testPixelShader.GetAddressOf());
		m_context->PSSetShader(m_testPixelShader.Get(), nullptr, 0);

		D3D11_INPUT_ELEMENT_DESC elemDesc = {};
		elemDesc.SemanticName = "POSITION";
		elemDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		elemDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elemDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		m_device->CreateInputLayout(&elemDesc, 1, vertexShader.c_str(), vertexShader.size(), m_testLayout.GetAddressOf());
		m_context->IASetInputLayout(m_testLayout.Get());

		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void Renderer::setDisplay(DisplayConfig& conf) {
		using enum DisplayConfig::Mode;

		m_window.setType(conf.mode);
		DXGI_MODE_DESC scDesc = {};

		switch (conf.mode) {
			case Windowed: {
				scDesc.Width = conf.width;
				scDesc.Height = conf.height;
				break;
			}

			case BorderlessFullscreen:
				RECT monitor = m_window.getMonitor();
				scDesc.Width = monitor.right - monitor.left;
				scDesc.Height = monitor.bottom - monitor.top;
				break;
		}

		m_sc->ResizeTarget(&scDesc);

		m_context->OMSetRenderTargets(0, nullptr, nullptr);
		if (m_outRTV)
			m_outRTV->Release();

		m_sc->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, NULL);
		ComPtr<ID3D11Texture2D> backBuff;
		m_sc->GetBuffer(0, __uuidof(backBuff), reinterpret_cast<void**>(backBuff.GetAddressOf()));
		D3D11_RENDER_TARGET_VIEW_DESC1 rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		m_device->CreateRenderTargetView1(backBuff.Get(), &rtvDesc, m_outRTV.GetAddressOf());

		D3D11_VIEWPORT vp = {};
		vp.Width = static_cast<float>(scDesc.Width);
		vp.Height = static_cast<float>(scDesc.Height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		m_context->RSSetViewports(1, &vp);
	}

	void Renderer::doFrame() {
		m_context->ClearRenderTargetView(m_outRTV.Get(), s_black);
		m_context->OMSetRenderTargets(1, reinterpret_cast<ID3D11RenderTargetView**>(m_outRTV.GetAddressOf()), nullptr);

		m_context->Draw(3, 0);

		m_sc->Present(0, NULL);
	}

	Renderer::~Renderer() {

	}
}