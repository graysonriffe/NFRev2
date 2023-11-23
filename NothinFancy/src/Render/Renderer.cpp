#include "pch.h"
#include "Renderer.h"

#include "nf/Utility/Util.h"
#include "Input/InputState.h"

using namespace DirectX;

extern "C" __declspec(dllexport) DWORD NvOptimusEnablement = 0x01;

namespace nf::render {
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

		XMMATRIX dummy = XMMatrixIdentity();
		m_viewProjBuffer = std::make_unique<Buffer>(m_device, Buffer::Type::Constant, &dummy, sizeof(dummy));

		//Test stuff

		std::string vertexShader, pixelShader;
		if (!util::readFile("BasicVertex.cso", vertexShader))
			NFError("Could not read BasicVertex.cso!");
		if (!util::readFile("BasicPixel.cso", pixelShader))
			NFError("Could not read BasicVertex.cso!");

		m_testShaders = std::make_unique<ShaderSet>(m_device, vertexShader, pixelShader);
		m_testShaders->bind(m_context);

		m_testLayout = std::make_unique<InputLayout>();
		m_testLayout->pushFloat("POSITION", 3);
		m_testLayout->pushFloat("NORMAL", 3);
		m_testLayout->pushFloat("TEXCOORD", 2);
		m_testLayout->create(m_device, vertexShader);
		m_testLayout->bind(m_context);

		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_testSampler = std::make_unique<SamplerState>(m_device);
		m_testSampler->bind(m_context);

		m_testBlendState = std::make_unique<BlendState>(m_device);
		m_testBlendState->bind(m_context);

		m_testModelConstantBuffer = std::make_unique<Buffer>(m_device, Buffer::Type::Constant, &dummy, sizeof(dummy));

		std::vector<ID3D11Buffer*> constantBuffers;
		constantBuffers.push_back(m_viewProjBuffer->getBuffer().Get());
		constantBuffers.push_back(m_testModelConstantBuffer->getBuffer().Get());
		m_context->VSSetConstantBuffers(0, static_cast<unsigned int>(constantBuffers.size()), constantBuffers.data());

		m_testFramebuffer = std::make_unique<Framebuffer>(m_device);

		m_testCamera = std::make_unique<Camera>(Vec3(0.0f, 0.0f, -3.0f));
		m_testCamera->setType(Camera::Type::FPS);

		std::string objData;
		if (!util::readFile("floor.obj", objData))
			NFError("Could not read floor.obj!");

		m_testFloor = std::make_unique<Model>(m_device, objData);

		std::string textureData;
		if (!util::readFile("floor.jpg", textureData))
			NFError("Could not read floor.jpg!");

		m_testFloorTexture = std::make_unique<Texture>(m_device, textureData);

		if (!util::readFile("cube.obj", objData))
			NFError("Could not read cube.obj!");

		m_testCube = std::make_unique<Model>(m_device, objData);

		if (!util::readFile("cube.png", textureData))
			NFError("Could not read cube.png!");

		m_testCubeTexture = std::make_unique<Texture>(m_device, textureData);

		if (!util::readFile("specular.png", textureData))
			NFError("Could not read specular.png!");

		m_testCubeSpecularTexture = std::make_unique<Texture>(m_device, textureData, Texture::Type::LINEAR);

		m_testLight = std::make_unique<Light>(Light::Type::Point, Vec3());
		m_testLight->setColor(Vec3(1.0f, 1.0f, 0.6f));
		XMVECTOR lightBuffDummy[3] = { XMVectorZero(), XMVectorZero(), XMVectorZero() };
		m_testLightConstantBuffer = std::make_unique<Buffer>(m_device, Buffer::Type::Constant, &lightBuffDummy, sizeof(lightBuffDummy));
		m_context->PSSetConstantBuffers(0, 1, m_testLightConstantBuffer->getBuffer().GetAddressOf());
	}

	void Renderer::setDisplay(DisplayConfig& conf) {
		using enum DisplayConfig::Mode;

		bool wasMouseCaptured = input::InputState::isMouseCapture();
		if (wasMouseCaptured)
			input::InputState::captureMouse(false);

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

		if (wasMouseCaptured)
			input::InputState::captureMouse(true);
	}

	void Renderer::doFrame() {
		DXGI_SWAP_CHAIN_DESC1 scDesc = {};
		m_sc->GetDesc1(&scDesc);
		unsigned int scWidth = scDesc.Width, scHeight = scDesc.Height;

		m_testFramebuffer->bind(m_context, m_device, scWidth, scHeight);
		m_testFramebuffer->clear(m_context, s_black);

		m_testCamera->update();
		Vec3 cameraPos = m_testCamera->getPosition(), cameraDir = m_testCamera->getRotation();

		static float x = 0.0f;
		x += 0.02f;
		m_testLight->setPosition(Vec3(5 * std::sin(x), 3.0f, 5 * std::cos(x)));
		std::vector<float> psBuff;
		Vec3 lightPos = m_testLight->getPosition();
		Vec3 lightColor = m_testLight->getColor();
		psBuff.push_back(lightPos.x);
		psBuff.push_back(lightPos.y);
		psBuff.push_back(lightPos.z);
		psBuff.push_back(1.0f);
		psBuff.push_back(lightColor.x);
		psBuff.push_back(lightColor.y);
		psBuff.push_back(lightColor.z);
		psBuff.push_back(1.0f);
		psBuff.push_back(cameraPos.x);
		psBuff.push_back(cameraPos.y);
		psBuff.push_back(cameraPos.z);
		psBuff.push_back(0.0f);

		m_testLightConstantBuffer->update(m_context, psBuff.data(), psBuff.size() * sizeof(float));

		XMVECTOR camera = XMVectorSet(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f), lookDir = XMVectorSet(cameraDir.x, cameraDir.y, cameraDir.z, 0.0f), up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMMATRIX view = XMMatrixLookToLH(camera, lookDir, up);
		XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, static_cast<float>(scWidth) / scHeight, 0.1f, 1000.0f);
		XMMATRIX vp = view * projection;
		m_viewProjBuffer->update(m_context, &vp, sizeof(vp));

		/*m_testFloor->bind(m_context);
		m_testFloorTexture->bind(m_context);
		XMMATRIX model = XMMatrixIdentity();
		model *= XMMatrixScaling(1.0f, 1.0f, 1.0f);
		model *= XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0.0f));
		model *= XMMatrixTranslation(0.0f, -3.0f, 0.0f);
		m_testModelConstantBuffer->update(m_context, &model, sizeof(model));
		m_context->DrawIndexed(m_testFloor->getIndexCount(), 0, 0);*/

		m_testCube->bind(m_context);
		//m_testCubeTexture->bind(m_context);
		std::vector<ID3D11ShaderResourceView*> cubeTextures;
		cubeTextures.push_back(m_testCubeTexture->getView().Get());
		cubeTextures.push_back(m_testCubeSpecularTexture->getView().Get());
		m_context->PSSetShaderResources(0, 2, cubeTextures.data());

		XMMATRIX model = XMMatrixIdentity();
		model *= XMMatrixScaling(1.0f, 1.0f, 1.0f);
		model *= XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0.0f));
		model *= XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		m_testModelConstantBuffer->update(m_context, &model, sizeof(model));
		m_context->DrawIndexed(m_testCube->getIndexCount(), 0, 0);

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