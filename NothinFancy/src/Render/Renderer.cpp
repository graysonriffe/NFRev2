#include "pch.h"
#include "Renderer.h"

#include "nf/Utility/Util.h"

namespace nf::render {
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
	}

	void Renderer::doFrame() {
		m_sc->Present(0, NULL);
	}

	Renderer::~Renderer() {

	}
}