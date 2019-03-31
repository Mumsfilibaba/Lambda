#include "SandBox.h"
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

		//Create commandlist
		{
			for (uint32 i = 0; i < 3; i++)
				m_pLists[i] = ICommandList::Create(COMMAND_LIST_TYPE_GRAPHICS);
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
			desc.Samples = 1;
			desc.MipMaps = 0;

			pDevice->CreateTexture2D(&m_pDepthBuffer, nullptr, desc);
		}
	}

	void SandBox::OnUpdate(Time dt)
	{
	}

	void SandBox::OnRender(Time dt)
	{
		IGraphicsDevice* pDevice = IGraphicsDevice::GetInstance();
		//Set commandlist for frame
		m_pCurrentList = m_pLists[pDevice->GetCurrentBackBufferIndex()];
		m_pCurrentList->Reset();

		//Set and clear rendertarget
		float color[] = { 0.392f, 0.584f, 0.929f, 1.0f };
		IRenderTarget* pRenderTarget = pDevice->GetCurrentRenderTarget();
		m_pCurrentList->TransitionResource(pRenderTarget, RESOURCE_STATE_RENDERTARGET);
		m_pCurrentList->ClearRenderTargetView(pRenderTarget, color);
		m_pCurrentList->SetRenderTarget(pRenderTarget);

		//Set scissor and 
		Math::Rectangle scissorrect;
		scissorrect.TopLeft.x = 0.0f;
		scissorrect.TopLeft.y = 0.0f;
		scissorrect.BottomRight.x = (float)GetWindow()->GetWidth();
		scissorrect.BottomRight.y = (float)GetWindow()->GetHeight();

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
		SafeRelease(m_pDepthBuffer);
	}
}