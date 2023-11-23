#pragma once
#include <d3d11_3.h>
#include <wrl/client.h>

#include "Window.h"

#include "ShaderSet.h"
#include "Buffer.h"
#include "InputLayout.h"
#include "SamplerState.h"
#include "Texture.h"
#include "BlendState.h"
#include "Framebuffer.h"
#include "Model.h"
#include "Camera.h"
#include "Light.h"

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
		void outputRTV(ComPtr<ID3D11RenderTargetView> rtv);

		Window& m_window;
		const UINT m_scFlags;

		ComPtr<ID3D11Device3> m_device;
		ComPtr<ID3D11DeviceContext4> m_context;
		ComPtr<IDXGISwapChain2> m_sc;
		ComPtr<ID3D11RenderTargetView1> m_outRTV;

		UP<Buffer> m_viewProjBuffer;

		//Test stuff
		UP<ShaderSet> m_testShaders;
		UP<InputLayout> m_testLayout;
		UP<SamplerState> m_testSampler;
		UP<BlendState> m_testBlendState;
		UP<Buffer> m_testModelConstantBuffer;
		UP<Framebuffer> m_testFramebuffer;

		UP<Camera> m_testCamera;
		UP<Model> m_testFloor;
		UP<Texture> m_testFloorTexture;
		UP<Model> m_testCube;
		UP<Texture> m_testCubeTexture;
		UP<Texture> m_testCubeSpecularTexture;
		UP<Light> m_testLight;
		UP<Buffer> m_testLightConstantBuffer;
	};
}