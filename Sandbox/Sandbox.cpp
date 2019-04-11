#include "SandBox.h"
#include <System/Log.h>
#include <Math/Math.h>

namespace Lambda
{
	SandBox::SandBox()
		: m_pCurrentList(nullptr),
		m_pVS(nullptr),
		m_pPS(nullptr),
		m_pVertexBuffer(nullptr),
		m_pPipelineState(nullptr),
		m_pDepthBuffer(nullptr),
		m_pLists()
	{
		for (uint32 i = 0; i < 3; i++)
			m_pLists[i] = nullptr;
	}
	

	SandBox::~SandBox()
	{
	}


	void SandBox::OnLoad()
	{
		using namespace Math;

		//Add Sandbox-level eventlayer
		EventLayer sandboxLayer = { SandBox::OnEvent, "SandBox" };
		EventDispatcher::PushEventLayer(sandboxLayer);

		//Init size
		m_Width = (float)GetWindow()->GetWidth();
		m_Height = (float)GetWindow()->GetHeight();

		//Create commandlist
		{
			for (uint32 i = 0; i < 3; i++)
				m_pLists[i] = ICommandList::Create(COMMAND_LIST_TYPE_GRAPHICS);

			m_pCurrentList = m_pLists[0];
			m_pCurrentList->Reset();
		}

		IGraphicsDevice* pDevice = IGraphicsDevice::GetInstance();
		//Create vertexshader
		{
			ShaderDesc desc = {};
			desc.Type = SHADER_TYPE_VERTEX;
#if defined(LAMBDA_DEBUG)
			desc.Flags = SHADER_FLAG_COMPILE_DEBUG;
#else
			desc.Flags = SHADER_FLAG_NONE;
#endif
			desc.pEntryPoint = "VSMain";
			pDevice->CreateShaderFromFile(&m_pVS, "Triangle.hlsl", desc);
		}

		//Create pixelshader
		{
			ShaderDesc desc = {};
			desc.Type = SHADER_TYPE_PIXEL;
#if defined(LAMBDA_DEBUG)
			desc.Flags = SHADER_FLAG_COMPILE_DEBUG;
#else
			desc.Flags = SHADER_FLAG_NONE;
#endif
			desc.pEntryPoint = "PSMain";
			pDevice->CreateShaderFromFile(&m_pPS, "Triangle.hlsl", desc);
		}

		//Create pipelinestate
		{
			GraphicsPipelineStateDesc desc = {};
			desc.pVertexShader = m_pVS;
			desc.pPixelShader = m_pPS;

			pDevice->CreateGraphicsPipelineState(&m_pPipelineState, desc);
		}

		//Create vertexbuffer
		{
			Vec3f vertices[] = 
			{
				Vec3f(0.0f, 0.5f, 0.0f),
				Vec3f(0.5f, -0.5f, 0.0f),
				Vec3f(-0.5f, -0.5f, 0.0f),
			};

			BufferDesc desc = {};
			desc.Usage = RESOURCE_USAGE_DEFAULT;
			desc.Flags = BUFFER_FLAGS_VERTEX_BUFFER;
			desc.SizeInBytes = sizeof(Vec3f) * 3;
			desc.StrideInBytes = sizeof(Vec3f);

			ResourceData data = {};
			data.pData = &vertices;
			data.SizeInBytes = desc.SizeInBytes;

			pDevice->CreateBuffer(&m_pVertexBuffer, &data, desc);
			m_pCurrentList->TransitionResource(m_pVertexBuffer, RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}

		//Create colorbuffer
		{
			Vec4f color = Vec4f(0.0f, 1.0f, 0.0f, 1.0f);

			BufferDesc desc = {};
			desc.Usage = RESOURCE_USAGE_DEFAULT;
			desc.Flags = BUFFER_FLAGS_CONSTANT_BUFFER;
			desc.SizeInBytes = (uint32)Math::AlignUp(sizeof(Vec4f), 256);
			desc.StrideInBytes = sizeof(Vec4f);

			ResourceData data = {};
			data.pData = &color;
			data.SizeInBytes = desc.SizeInBytes;

			pDevice->CreateBuffer(&m_pColorBuffer, &data, desc);
		}

		//Create camerabuffer
		{
			CreateCamera(m_Width, m_Height);

			BufferDesc desc = {};
			desc.Usage = RESOURCE_USAGE_DEFAULT;
			desc.Flags = BUFFER_FLAGS_CONSTANT_BUFFER;
			desc.SizeInBytes = (uint32)Math::AlignUp(sizeof(CameraBuffer), 256);
			desc.StrideInBytes = sizeof(CameraBuffer);

			ResourceData data = {};
			data.pData = &m_Camera;
			data.SizeInBytes = desc.SizeInBytes;

			pDevice->CreateBuffer(&m_pCameraBuffer, &data, desc);
			m_pCurrentList->TransitionResource(m_pCameraBuffer, RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}

		//Create depthbuffer
		{
			Texture2DDesc desc = {};
			desc.Usage = RESOURCE_USAGE_DEFAULT;
			desc.Flags = TEXTURE_FLAGS_DEPTH_STENCIL;
			desc.ArraySize = 1;
			desc.Width = GetWindow()->GetWidth();
			desc.Height = GetWindow()->GetHeight();
			desc.Format = FORMAT_D24_UNORM_S8_UINT;
			desc.SampleCount = 1;
			desc.MipLevels = 0;
			desc.ClearDepth = 1.0f;
			desc.ClearStencil = 0;

			pDevice->CreateTexture2D(&m_pDepthBuffer, nullptr, desc);
		}

		m_pCurrentList->Close();
		pDevice->ExecuteCommandList(&m_pCurrentList, 1);
	}


	void SandBox::OnUpdate(Time dt)
	{
	}


	void SandBox::OnRender(Time dt)
	{
		using namespace Math;

		IGraphicsDevice* pDevice = IGraphicsDevice::GetInstance();
		//Set commandlist for frame
		m_pCurrentList = m_pLists[pDevice->GetCurrentBackBufferIndex()];
		m_pCurrentList->Reset();

		//Set and clear rendertarget
		float color[] = { 0.392f, 0.584f, 0.929f, 1.0f };
		ITexture2D* pRenderTarget = pDevice->GetCurrentRenderTarget();
		
		m_pCurrentList->TransitionResource(pRenderTarget, RESOURCE_STATE_RENDERTARGET);
		m_pCurrentList->TransitionResource(m_pDepthBuffer, RESOURCE_STATE_DEPTH_WRITE);

		m_pCurrentList->ClearRenderTarget(pRenderTarget, color);
		m_pCurrentList->ClearDepthStencil(m_pDepthBuffer, 1.0f, 0);
		m_pCurrentList->SetRenderTarget(pRenderTarget, m_pDepthBuffer);

		//Set scissor and viewport
		Math::Rectangle scissorrect;
		scissorrect.TopLeft.x = 0.0f;
		scissorrect.TopLeft.y = 0.0f;
		scissorrect.BottomRight.x = m_Width;
		scissorrect.BottomRight.y = m_Height;

		Viewport viewport;
		viewport.Width = scissorrect.BottomRight.x;
		viewport.Height = scissorrect.BottomRight.y;
		viewport.TopX = 0.0f;
		viewport.TopY = 0.0f;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		
		m_pCurrentList->SetViewport(viewport);
		m_pCurrentList->SetScissorRect(scissorrect);

		//Set pipelinestate and topology
		m_pCurrentList->SetPrimtiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pCurrentList->SetGraphicsPipelineState(m_pPipelineState);

		//Set constantbuffers
		static Random random;
		static Vec4f colorBuff = random.GenerateVector4();
		static float timer = 0.0f;
		timer += dt.AsSeconds();

		if (timer >= 1.0f)
		{
			colorBuff = random.GenerateVector4();
			timer = 0.0f;

			LOG_DEBUG_INFO("%s\n", ToString(colorBuff).c_str());
		}

		ResourceData data = {};
		data.pData = &colorBuff;
		data.SizeInBytes = sizeof(Vec4f);

		m_pCurrentList->TransitionResource(m_pColorBuffer, RESOURCE_STATE_COPY_DEST);
		m_pCurrentList->UpdateBuffer(m_pColorBuffer, &data);
		m_pCurrentList->TransitionResource(m_pColorBuffer, RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		
		m_pCurrentList->VSSetConstantBuffers(&m_pCameraBuffer, 1, 0);
		m_pCurrentList->PSSetConstantBuffers(&m_pColorBuffer, 1, 0);

		//Set vertexbuffer
		m_pCurrentList->SetVertexBuffer(m_pVertexBuffer, 0);
		m_pCurrentList->DrawInstanced(3, 1, 0, 0);

		m_pCurrentList->TransitionResource(pRenderTarget, RESOURCE_STATE_PRESENT_COMMON);

		m_pCurrentList->Close();

		//Present
		pDevice->ExecuteCommandList(&m_pCurrentList, 1);
		pDevice->Present(0);
		pDevice->GPUWaitForFrame();
	}


	void SandBox::OnRelease()
	{
		IGraphicsDevice* pDevice = IGraphicsDevice::GetInstance();
		pDevice->WaitForGPU();

		for (uint32 i = 0; i < 3; i++)
			SafeRelease(m_pLists[i]);

		SafeRelease(m_pVS);
		SafeRelease(m_pPS);
		SafeRelease(m_pPipelineState);
		SafeRelease(m_pVertexBuffer);
		SafeRelease(m_pColorBuffer);
		SafeRelease(m_pDepthBuffer);
	}


	void SandBox::CreateCamera(uint32 width, uint32 height)
	{
		using namespace Math;

		m_Camera.View = Store(Float4x4::LookAt(Vec3Const::UP, Vec3f(0.0f), Vec3f(0.0f, 0.0f, -1.0f)).Transpose());
		m_Camera.Proj = Store(Float4x4::Perspective(ToRadiansF(90.0f), (float)width / height, 10.0f, 0.01f).Transpose());
	}


	bool SandBox::OnEvent(const Event& event)
	{
		if (event.Type == EVENT_TYPE_WINDOW_RESIZE)
		{
			SandBox& instance = (SandBox&)GetInstance();
			SafeRelease(instance.m_pDepthBuffer);

			Texture2DDesc desc = {};
			desc.Usage = RESOURCE_USAGE_DEFAULT;
			desc.Flags = TEXTURE_FLAGS_DEPTH_STENCIL;
			desc.ArraySize = 1;
			desc.Width = event.WindowResize.Width;
			desc.Height = event.WindowResize.Height;
			desc.Format = FORMAT_D24_UNORM_S8_UINT;
			desc.SampleCount = 1;
			desc.MipLevels = 0;
			desc.ClearDepth = 1.0f;
			desc.ClearStencil = 0;

			//TODO: needs to release the descriptor in DX12-backend
			IGraphicsDevice* pDevice = IGraphicsDevice::GetInstance();
			pDevice->CreateTexture2D(&instance.m_pDepthBuffer, nullptr, desc);

			LOG_DEBUG_INFO("Resized depthbuffer");

			//Set size variable
			instance.m_Width = (float)event.WindowResize.Width;
			instance.m_Height = (float)event.WindowResize.Height;

			//Update camera
			instance.CreateCamera(event.WindowResize.Width, event.WindowResize.Height);

			ResourceData data = {};
			data.pData = &instance.m_Camera;
			data.SizeInBytes = sizeof(CameraBuffer);
			
			instance.m_pCurrentList->Reset();

			instance.m_pCurrentList->TransitionResource(instance.m_pCameraBuffer, RESOURCE_STATE_COPY_DEST);
			instance.m_pCurrentList->UpdateBuffer(instance.m_pCameraBuffer, &data);
			instance.m_pCurrentList->TransitionResource(instance.m_pCameraBuffer, RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

			instance.m_pCurrentList->Close();
			pDevice->ExecuteCommandList(&instance.m_pCurrentList, 1);
		}

		return false;
	}
}