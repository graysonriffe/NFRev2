#include "pch.h"
#include "Renderer.h"

#include "nf/Utility/Util.h"

using namespace DirectX;

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

		//Test stuff

		std::string vertexShader, pixelShader;
		util::readFile("BasicVertex.cso", vertexShader);
		util::readFile("BasicPixel.cso", pixelShader);

		m_testShaders = std::make_unique<ShaderSet>(m_device, vertexShader, pixelShader);
		m_testShaders->bind(m_context);

		float cube[] = {
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f,  0.5f, -0.5f, 0.0f, 0.0f,
			0.5f,  0.5f, -0.5f, 1.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 0.0f, 1.0f,
			0.5f,  0.5f, 0.5f, 0.0f, 0.0f,
			-0.5f,  0.5f, 0.5f, 1.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, 0.5f,  0.5f, 0.0f, 0.0f,
			0.5f, 0.5f,  0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f, 0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, 1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f,  0.5f, -0.5f, 0.0f, 0.0f,
			0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
			0.5f, -0.5f,  0.5f, 1.0f, 1.0f,
		};

		unsigned int indices[] = {
			0,  1,  2, 0,  2,  3,
			4,  5,  6, 4,  6,  7,
			8,  9, 10, 8, 10, 11,
			12, 13, 14, 12, 14, 15,
			16, 17, 18, 16, 18, 19,
			20, 21, 22, 20, 22, 23
		};

		m_testBuffer = std::make_unique<Buffer>(m_device, Buffer::Type::Vertex, cube, sizeof(cube), 5 * sizeof(float));
		m_testBuffer->bind(m_context);
		m_testIndexBuffer = std::make_unique<Buffer>(m_device, Buffer::Type::Index, indices, sizeof(indices));
		m_testIndexBuffer->bind(m_context);

		m_testLayout = std::make_unique<InputLayout>();
		m_testLayout->pushFloat("POSITION", 3);
		m_testLayout->pushFloat("TEXCOORD", 2);
		m_testLayout->create(m_device, vertexShader);
		m_testLayout->bind(m_context);

		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		XMMATRIX dummy = XMMatrixIdentity();
		m_testConstantBuffer = std::make_unique<Buffer>(m_device, Buffer::Type::Constant, &dummy, sizeof(dummy));

		m_testSampler = std::make_unique<SamplerState>(m_device);
		m_testSampler->bind(m_context);

		std::string textureData;
		if (!util::readFile("test.png", textureData))
			NFError("Could not read test.png!");

		m_testTexture = std::make_unique<Texture>(m_device, textureData);

		m_testBlendState = std::make_unique<BlendState>(m_device);
		m_testBlendState->bind(m_context);
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
		DXGI_SWAP_CHAIN_DESC1 scDesc = {};
		m_sc->GetDesc1(&scDesc);
		unsigned int scWidth = scDesc.Width, scHeight = scDesc.Height;

		m_context->ClearRenderTargetView(m_outRTV.Get(), s_black);
		m_context->OMSetRenderTargets(1, reinterpret_cast<ID3D11RenderTargetView**>(m_outRTV.GetAddressOf()), nullptr);

		m_testTexture->bind(m_context);

		XMMATRIX model = XMMatrixIdentity();
		model *= XMMatrixScaling(1.0f, 1.0f, 1.0f);
		static float x = 0.0f;
		x += 0.01f;
		float offset = XM_PI * std::sin(x);
		model *= XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0.2f, 0.5f, 0.7f, 0.0f), offset));
		model *= XMMatrixTranslation(0.0f, 0.0f, 1.5f);

		XMVECTOR camera = XMVectorZero(), lookDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMMATRIX view = XMMatrixLookToLH(camera, lookDir, up);
		XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, static_cast<float>(scWidth) / scHeight, 0.1f, 1000.0f);
		XMMATRIX mvp = model * view * projection;

		m_testConstantBuffer->update(m_context, &mvp, sizeof(mvp));
		m_testConstantBuffer->bind(m_context);

		m_context->DrawIndexed(36, 0, 0);

		m_sc->Present(1, NULL);
	}

	Renderer::~Renderer() {

	}
}