#include "LambdaPch.h"
#include "Graphics/Renderer3D.h"
#include "Graphics/IGraphicsDevice.h"
#include "Graphics/IBuffer.h"
#include "System/Application.h"

namespace Lambda
{
	Renderer3D::Renderer3D()
		: m_pCurrentList(nullptr),
		m_Lists()
	{
	}


	void Renderer3D::Init()
	{
		IGraphicsDevice* pDevice = IGraphicsDevice::Get();
		//Create commandlists
		for (uint32 i = 0; i < 3; i++)
		{
			ICommandList* pList = nullptr;
			pDevice->CreateCommandList(&pList, COMMAND_LIST_TYPE_GRAPHICS);
			if (pList)
			{
				std::string name = "CommandList [" + std::to_string(i) + "]";
				pList->SetName(name.c_str());

				m_Lists.emplace_back(pList);
			}
		}

		//Reset current list
		m_pCurrentList = m_Lists[0];
		m_pCurrentList->Reset();

		Application& app = Application::Get();
		//Create backbuffer renderpass
		RenderPassDesc desc = {};
		desc.SampleCount						= app.GetEngineParams().SampleCount;
		desc.NumRenderTargets					= 1;
		desc.RenderTargets[0].Format			= pDevice->GetBackBufferFormat();
		desc.RenderTargets[0].Flags				= RENDER_PASS_ATTACHMENT_FLAG_RESOLVE;
		desc.RenderTargets[0].LoadOperation		= LOAD_OP_CLEAR;
		desc.RenderTargets[0].StoreOperation	= STORE_OP_STORE;
		desc.RenderTargets[0].FinalState		= RESOURCE_STATE_RENDERTARGET_PRESENT;
		desc.DepthStencil.Format				= FORMAT_D24_UNORM_S8_UINT;
		desc.DepthStencil.Flags					= 0;
		desc.DepthStencil.LoadOperation			= LOAD_OP_CLEAR;
		desc.DepthStencil.StoreOperation		= STORE_OP_UNKNOWN;
		desc.DepthStencil.FinalState			= RESOURCE_STATE_DEPTH_STENCIL;
		pDevice->CreateRenderPass(&m_pRenderPass, desc);
		app.GetUILayer()->Init(m_pRenderPass, m_pCurrentList);

		//Execute commandlist
		m_pCurrentList->Close();
		pDevice->ExecuteCommandList(&m_pCurrentList, 1);
		pDevice->WaitForGPU();
	}


	void Renderer3D::Begin()
	{
		//Get current device
		IGraphicsDevice* pDevice = IGraphicsDevice::Get();
		//Set commandlist for frame
		uint32 backBufferIndex = pDevice->GetBackBufferIndex();
		m_pCurrentList = m_Lists[backBufferIndex];
		m_pCurrentList->Reset();
		
		//Begin present
		pDevice->PresentBegin();

		//Get rendertarget
		float color[] = { 0.392f, 0.584f, 0.929f, 1.0f };
		ITexture* pRenderTarget = pDevice->GetRenderTarget();
		ITexture* pDepthBuffer = pDevice->GetDepthStencil();

		//Set rendertargets and clearcolors
		m_pRenderPass->SetRenderTargets(&pRenderTarget, 1, pDepthBuffer);
		m_pRenderPass->SetClearValues(color, 1.0f, 0);
	}


	void Renderer3D::Submit(IBuffer* pVertexBuffer, IBuffer* pIndexBuffer, IGraphicsPipelineState* pPipelineState) const
	{
        m_pCurrentList->SetVertexBuffer(pVertexBuffer, 0);
        m_pCurrentList->SetIndexBuffer(pIndexBuffer, FORMAT_R32_FLOAT);
        m_pCurrentList->SetGraphicsPipelineState(pPipelineState);
        
        BufferDesc desc     = pIndexBuffer->GetDesc();
        uint32 indexCount   = desc.SizeInBytes / desc.StrideInBytes;
        m_pCurrentList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
	}


	void Renderer3D::End()
	{
		m_pCurrentList->Close();
	}


	void Renderer3D::Swapbuffers()
	{
		//Get current device
		IGraphicsDevice* pDevice = IGraphicsDevice::Get();
		pDevice->PresentEnd(&m_pCurrentList, 1);
	}


	void Renderer3D::Release()
	{
		//Release commandlists
		IGraphicsDevice* pDevice = IGraphicsDevice::Get();
		for (auto& list : m_Lists)
			pDevice->DestroyCommandList(&list);

		m_Lists.clear();
		//Release renderpass
		pDevice->DestroyRenderPass(&m_pRenderPass);
	}
	
	
	IRenderPass* Renderer3D::GetRenderPass() const
	{
		return m_pRenderPass;
	}
}
