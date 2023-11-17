#include "pch.h"
#include "Renderer.h"

#include "nf/Utility/Util.h"

using namespace DirectX;

extern "C" __declspec(dllexport) DWORD NvOptimusEnablement = 0x01;

namespace nf::render {
	static void parseOBJ(const std::string& obj, std::vector<float>& verticesOut, std::vector<unsigned int>& indicesOut) {
		verticesOut.clear();
		indicesOut.clear();

		std::stringstream ss(obj);

		struct VPos {
			float x, y, z;
		};
		std::vector<VPos> rawV;

		struct TCoord {
			float u, v;
		};
		std::vector<TCoord> rawTC;

		struct VInd {
			unsigned int v, t;
		};
		std::vector<VInd> rawInd;

		std::string line;
		while (std::getline(ss, line)) {
			std::stringstream ssLine(line);
			std::string op;
			ssLine >> op;

			if (op == "v") {
				float x = 0.0f, y = 0.0f, z = 0.0f;
				ssLine >> x >> y >> z;
				rawV.push_back({ x, y, z });
			}
			else if (op == "vt") {
				float u = 0.0f, v = 0.0f;
				ssLine >> u >> v;
				rawTC.push_back({ u, v });
			}
			else if (op == "f") {
				unsigned int v1 = 0, t1 = 0, v2 = 0, t2 = 0, v3 = 0, t3 = 0;
				char temp = 0;
				ssLine >> v1 >> temp >> t1 >> v2 >> temp >> t2 >> v3 >> temp >> t3;
				rawInd.push_back({ v1, t1 });
				rawInd.push_back({ v2, t2 });
				rawInd.push_back({ v3, t3 });
			}
		}

		struct Vertex {
			VPos pos;
			TCoord tc;

			bool operator<(const Vertex other) const {
				return std::memcmp(this, &other, sizeof(Vertex)) > 0;
			}
		};

		std::map<Vertex, unsigned int> vMap;
		unsigned int currIndex = 0;
		for (const auto& currInd : rawInd) {
			Vertex currVertex = { rawV[currInd.v - 1], rawTC[currInd.t - 1] };

			bool found = vMap.find(currVertex) != vMap.end();
			if (found) {
				indicesOut.push_back(vMap[currVertex]);
				continue;
			}

			verticesOut.push_back(currVertex.pos.x);
			verticesOut.push_back(currVertex.pos.y);
			verticesOut.push_back(currVertex.pos.z);
			verticesOut.push_back(currVertex.tc.u);
			verticesOut.push_back(currVertex.tc.v);
			indicesOut.push_back(currIndex);
			vMap[currVertex] = currIndex;
			currIndex++;
		}
	}

	static const float s_black[] = {
		0.0f, 0.0f, 0.0f, 1.0f
	};

	Renderer::Renderer(Window& window, DisplayConfig& dispConfig)
		: m_window(window)
		, m_scFlags(DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING)
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
		scDesc.Flags = m_scFlags;

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

		std::string objData;
		if (!util::readFile("test.obj", objData))
			NFError("Could not read test.obj!");

		std::vector<float> objVertices;
		std::vector<unsigned int> objIndices;

		parseOBJ(objData, objVertices, objIndices);

		m_testBuffer = std::make_unique<Buffer>(m_device, Buffer::Type::Vertex, objVertices.data(), sizeof(float) * objVertices.size(), 5 * sizeof(float));
		m_testBuffer->bind(m_context);
		m_testIndexBuffer = std::make_unique<Buffer>(m_device, Buffer::Type::Index, objIndices.data(), sizeof(unsigned int) * objIndices.size());
		m_testIndexBuffer->bind(m_context);

		m_testLayout = std::make_unique<InputLayout>();
		m_testLayout->pushFloat("POSITION", 3);
		m_testLayout->pushFloat("TEXCOORD", 2);
		m_testLayout->create(m_device, vertexShader);
		m_testLayout->bind(m_context);

		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		XMMATRIX dummy = XMMatrixIdentity();
		m_testConstantBuffer = std::make_unique<Buffer>(m_device, Buffer::Type::Constant, &dummy, sizeof(dummy));
		m_testConstantBuffer->bind(m_context);

		m_testSampler = std::make_unique<SamplerState>(m_device);
		m_testSampler->bind(m_context);

		std::string textureData;
		if (!util::readFile("test.png", textureData))
			NFError("Could not read test.png!");

		m_testTexture = std::make_unique<Texture>(m_device, textureData.data(), textureData.size());

		m_testBlendState = std::make_unique<BlendState>(m_device);
		m_testBlendState->bind(m_context);

		m_testFramebuffer = std::make_unique<Framebuffer>(m_device, 2);
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

		m_sc->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, m_scFlags);
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

		m_window.show();
	}

	void Renderer::doFrame() {
		DXGI_SWAP_CHAIN_DESC1 scDesc = {};
		m_sc->GetDesc1(&scDesc);
		unsigned int scWidth = scDesc.Width, scHeight = scDesc.Height;

		m_testFramebuffer->bind(m_context, m_device, scWidth, scHeight);
		m_testFramebuffer->clear(m_context, s_black);

		m_testTexture->bind(m_context);

		XMMATRIX model = XMMatrixIdentity();
		model *= XMMatrixScaling(1.0f, 1.0f, 1.0f);
		static float x = 0.0f;
		x += 0.01f;
		float offset = XM_PI * std::sin(x);
		model *= XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0.4f, 0.5f, 0.8f, 0.0f), offset));
		model *= XMMatrixTranslation(0.0f, 0.0f, 2.0f);

		XMVECTOR camera = XMVectorZero(), lookDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMMATRIX view = XMMatrixLookToLH(camera, lookDir, up);
		XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, static_cast<float>(scWidth) / scHeight, 0.1f, 1000.0f);
		XMMATRIX mvp = model * view * projection;

		m_testConstantBuffer->update(m_context, &mvp, sizeof(mvp));

		m_context->DrawIndexed(3642, 0, 0);

		//Copy testFramebuffer to outRTV
		outputRTV(m_testFramebuffer->getRTV());

		m_sc->Present(1, NULL);
	}

	void Renderer::outputRTV(ComPtr<ID3D11RenderTargetView> rtv) {
		ComPtr<ID3D11Resource> in, out;
		rtv->GetResource(in.GetAddressOf());
		m_outRTV->GetResource(out.GetAddressOf());

		m_context->CopyResource(out.Get(), in.Get());
	}

	Renderer::~Renderer() {

	}
}