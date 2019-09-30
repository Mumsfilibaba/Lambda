#include "LambdaPch.h"
#include "Graphics/Renderer3D.h"
#include "Graphics/IDevice.h"
#include "Graphics/IBuffer.h"
#include "Graphics/IQuery.h"
#include "Graphics/Camera.h"
#include "Graphics/IPipelineResourceState.h"
#include "System/Application.h"
#include <glm/gtc/type_ptr.hpp>

namespace Lambda
{
	//----------
	//Renderer3D
	//----------

	Renderer3D::Renderer3D()
		: m_pCurrentList(nullptr),
		m_Lists(),
		m_Queries(),
		m_pCurrentQuery(nullptr),
		m_RenderPass(nullptr),
		m_LightBuffer(nullptr),
		m_CameraBuffer(nullptr),
		m_TransformBuffer(nullptr),
		m_MaterialBuffer(nullptr),
		m_Viewport(),
		m_ScissorRect(),
		m_FrameInfo(),
		m_FrameClock(),
		m_CurrentFPS(0)
	{
	}


	void Renderer3D::Init()
	{
		IDevice* pDevice = IDevice::Get();
		const DeviceDesc& deviceDesc = pDevice->GetDesc();

		for (uint32 i = 0; i < deviceDesc.BackBufferCount; i++)
		{
			//Create commandlists
			ICommandList* pList = nullptr;
			pDevice->CreateCommandList(&pList, COMMAND_LIST_TYPE_GRAPHICS);
			if (pList)
			{
				std::string name = "CommandList [" + std::to_string(i) + "]";
				pList->SetName(name.c_str());

				m_Lists.emplace_back(pList);
			}

			//Create queries for timesteps
			IQuery* pQuery			= nullptr;
			QueryDesc queryDesc		= {};
			queryDesc.Type			= QUERY_TYPE_TIMESTAMP;
			queryDesc.QueryCount	= 2;
			pDevice->CreateQuery(&pQuery, queryDesc);
			m_Queries.emplace_back(pQuery);
		}

		//Reset current list
		m_pCurrentList = m_Lists[0].Get();
		m_pCurrentList->Reset();

		//Current query
		m_pCurrentQuery = m_Queries[0].Get();

		Application& app = Application::Get();
		//Create backbuffer renderpass
		RenderPassDesc renderPassDesc = {};
		renderPassDesc.SampleCount						= app.GetEngineParams().SampleCount;
		renderPassDesc.NumRenderTargets					= 1;
		renderPassDesc.RenderTargets[0].Format			= pDevice->GetBackBufferFormat();
		renderPassDesc.RenderTargets[0].Flags			= RENDER_PASS_ATTACHMENT_FLAG_RESOLVE;
		renderPassDesc.RenderTargets[0].LoadOperation	= LOAD_OP_CLEAR;
		renderPassDesc.RenderTargets[0].StoreOperation	= STORE_OP_STORE;
		renderPassDesc.RenderTargets[0].FinalState		= RESOURCE_STATE_RENDERTARGET_PRESENT;
		renderPassDesc.DepthStencil.Format				= FORMAT_D24_UNORM_S8_UINT;
		renderPassDesc.DepthStencil.Flags				= 0;
		renderPassDesc.DepthStencil.LoadOperation		= LOAD_OP_CLEAR;
		renderPassDesc.DepthStencil.StoreOperation		= STORE_OP_UNKNOWN;
		renderPassDesc.DepthStencil.FinalState			= RESOURCE_STATE_DEPTH_STENCIL;
		pDevice->CreateRenderPass(&m_RenderPass, renderPassDesc);
		app.GetUILayer()->Init(m_RenderPass.Get(), m_pCurrentList);

		//Create camerabuffer
		BufferDesc cameraBufferdesc = {};
		cameraBufferdesc.pName			= "CameraBuffer";
		cameraBufferdesc.Usage			= RESOURCE_USAGE_DEFAULT;
		cameraBufferdesc.Flags			= BUFFER_FLAGS_CONSTANT_BUFFER;
		cameraBufferdesc.SizeInBytes	= sizeof(CameraBuffer);
		cameraBufferdesc.StrideInBytes	= sizeof(CameraBuffer);
		pDevice->CreateBuffer(&m_CameraBuffer, nullptr, cameraBufferdesc);

		//Create lightbuffer
		BufferDesc lightBufferDesc = {};
		lightBufferDesc.pName			= "LightBuffer";
		lightBufferDesc.Usage			= RESOURCE_USAGE_DEFAULT;
		lightBufferDesc.Flags			= BUFFER_FLAGS_CONSTANT_BUFFER;
		lightBufferDesc.SizeInBytes		= sizeof(LightBuffer);
		lightBufferDesc.StrideInBytes	= sizeof(LightBuffer);
		pDevice->CreateBuffer(&m_LightBuffer, nullptr, lightBufferDesc);

		//Create TransformBuffer
		BufferDesc transformBufferdesc = {};
		transformBufferdesc.pName			= "TransformBuffer";
		transformBufferdesc.Usage			= RESOURCE_USAGE_DYNAMIC;
		transformBufferdesc.Flags			= BUFFER_FLAGS_CONSTANT_BUFFER;
		transformBufferdesc.SizeInBytes		= sizeof(TransformBuffer);
		transformBufferdesc.StrideInBytes	= sizeof(TransformBuffer);

		ResourceData data = {};
		data.pData			= glm::value_ptr(glm::mat4(1.0f));
		data.SizeInBytes	= transformBufferdesc.SizeInBytes;

		pDevice->CreateBuffer(&m_TransformBuffer, &data, transformBufferdesc);

		//Create materialbuffer
		BufferDesc materialBufferDesc = {};
		materialBufferDesc.pName			= "MaterialBuffer";
		materialBufferDesc.Usage			= RESOURCE_USAGE_DYNAMIC;
		materialBufferDesc.Flags			= BUFFER_FLAGS_CONSTANT_BUFFER;
		materialBufferDesc.SizeInBytes		= sizeof(MaterialBuffer);
		materialBufferDesc.StrideInBytes	= sizeof(MaterialBuffer);
		pDevice->CreateBuffer(&m_MaterialBuffer, &data, materialBufferDesc);

		//Execute commandlist
		m_pCurrentList->Close();
		pDevice->ExecuteCommandList(&m_pCurrentList, 1);
		pDevice->WaitForGPU();

		//Setup viewport
		IWindow* pWindow = Application::Get().GetWindow();
		SetDisplaySize(pWindow->GetWidth(), pWindow->GetHeight());
	}


	void Renderer3D::Begin()
	{
		//Calculate number of frame per second
		m_FrameClock.Tick();
		m_FrameInfo.CPUTime = m_FrameClock.GetDeltaTime();

		if (m_FrameClock.GetTotalTime().AsSeconds() >= 1.0f)
		{
			m_FrameInfo.FPS = m_CurrentFPS;
			m_FrameClock.Reset();
			m_CurrentFPS = 0;
		}
		m_CurrentFPS++;

		//Get current device
		IDevice* pDevice = IDevice::Get();
		//Set commandlist for frame
		uint32 backBufferIndex = pDevice->GetBackBufferIndex();
		m_pCurrentList = m_Lists[backBufferIndex].Get();
		m_pCurrentList->Reset();
		
		//Get last frame's values from query
		uint64 values[2] = { 0, 0 };
		m_pCurrentQuery->GetResults(values, 2, 0);
		m_FrameInfo.GPUTime = Timestep(values[1] - values[0]);
		//Reset new query
		m_pCurrentQuery = m_Queries[backBufferIndex].Get();
		m_pCurrentList->ResetQuery(m_pCurrentQuery);

		//Begin present
		pDevice->PresentBegin();

		//Get rendertarget
		float color[] = { 0.392f, 0.584f, 0.929f, 1.0f };
		ITexture* pRenderTarget = pDevice->GetRenderTarget();
		ITexture* pDepthBuffer = pDevice->GetDepthStencil();

		//Set rendertargets and clearcolors
		m_RenderPass->SetRenderTargets(&pRenderTarget, 1, pDepthBuffer);
		m_RenderPass->SetClearValues(color, 1.0f, 0);
	}

	
	void Renderer3D::BeginScene(const Camera& camera, const LightBuffer& light)
	{
		//Update camerabuffer
		CameraBuffer cameraBuffer = {};
		cameraBuffer.View		= camera.GetView();
		cameraBuffer.Projection	= camera.GetProjection();
		cameraBuffer.Position	= camera.GetPosition();
		
		ResourceData cameraData = {};
		cameraData.pData		= &cameraBuffer;
		cameraData.SizeInBytes	= sizeof(CameraBuffer);
		m_pCurrentList->UpdateBuffer(m_CameraBuffer.Get(), &cameraData);

		//Update lightbuffer
		ResourceData lightData = {};
		lightData.pData			= &light;
		lightData.SizeInBytes	= sizeof(LightBuffer);
		m_pCurrentList->UpdateBuffer(m_LightBuffer.Get(), &lightData);

		//Begin renderpass
		m_pCurrentList->BeginRenderPass(m_RenderPass.Get());
		//Write query
		m_pCurrentList->WriteTimeStamp(m_pCurrentQuery, PIPELINE_STAGE_VERTEX);
		//Set viewport
		m_pCurrentList->SetViewport(m_Viewport);
		m_pCurrentList->SetScissorRect(m_ScissorRect);
	}


	void Renderer3D::Submit(const Model& model, const Material& material, const TransformBuffer& transform)
	{
		//Update transform
		ResourceData transformData = {};
		transformData.pData			= &transform;
		transformData.SizeInBytes	= sizeof(TransformBuffer);
		m_pCurrentList->UpdateBuffer(m_TransformBuffer.Get(), &transformData);
		
        //Update material
		MaterialBuffer materialBuffer = {};
		materialBuffer.Color		= material.Color;
		materialBuffer.HasAlbedoMap = material.pAlbedoMap ? 1 : 0;
		materialBuffer.HasNormalMap = material.pNormalMap ? 1 : 0;

		ResourceData materialData = {};
		materialData.pData			= &materialBuffer;
		materialData.SizeInBytes	= sizeof(MaterialBuffer);
		m_pCurrentList->UpdateBuffer(m_MaterialBuffer.Get(), &materialData);
		//Set resources
		IBuffer* ppBuffers[]		     = { m_CameraBuffer.Get(), m_TransformBuffer.Get(), m_MaterialBuffer.Get(), m_LightBuffer.Get() };
		ITexture* ppTextures[]		     = { material.pAlbedoMap, material.pNormalMap };
		ISamplerState* ppSamplerStates[] = { material.pSamplerState };
		material.pResourceState->SetConstantBuffers(ppBuffers, 4, 0);
		material.pResourceState->SetTextures(ppTextures, 2, 4);
		material.pResourceState->SetSamplerStates(ppSamplerStates, 1, 6);
		m_pCurrentList->SetGraphicsPipelineResourceState(material.pResourceState);
		//Set mesh
        m_pCurrentList->SetVertexBuffer(model.pVertexBuffer, 0);
        m_pCurrentList->SetIndexBuffer(model.pIndexBuffer, FORMAT_R32_UINT);
        //Set pipelinestate
		m_pCurrentList->SetGraphicsPipelineState(material.pPipelineState);
        //Draw
        m_pCurrentList->DrawIndexedInstanced(model.IndexCount, 1, 0, 0, 0);
	}


	void Renderer3D::EndScene() const
	{
		//Draw UI before ending renderpass
		Application::Get().GetUILayer()->Draw(m_pCurrentList);
		//End renderpass
		m_pCurrentList->EndRenderPass();
		//End by writing a timestamp
		m_pCurrentList->WriteTimeStamp(m_pCurrentQuery, PIPELINE_STAGE_PIXEL);
	}


	void Renderer3D::End()
	{
		m_pCurrentList->Close();
	}


	void Renderer3D::Swapbuffers()
	{
		//Get current device
		IDevice* pDevice = IDevice::Get();
		pDevice->PresentEnd(&m_pCurrentList, 1);
	}


	void Renderer3D::Release()
	{
		//Release commandlists
		for (auto& list : m_Lists)
			list.Release();
		m_Lists.clear();
		//Release queries
		for (auto& query : m_Queries)
			query.Get();
		m_Queries.clear();
		//Release renderpass
		m_RenderPass.Release();
		//Destroy camerabuffer
		m_CameraBuffer.Release();
		//Destroy lightbuffer
		m_LightBuffer.Release();
		//Destroy transformbuffer
		m_TransformBuffer.Release();
		//Destroy materialbuffer
		m_MaterialBuffer.Release();
	}

	
	void Renderer3D::SetDisplaySize(uint32 width, uint32 height)
	{
		//Viewport
		m_Viewport.Width	= float(width);
		m_Viewport.Height	= float(height);
		m_Viewport.MaxDepth = 1.0f;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.TopX		= 0.0f;
		m_Viewport.TopY		= 0.0f;

		//Scissorrect
		m_ScissorRect.Width  = float(width);
		m_ScissorRect.Height = float(height);
		m_ScissorRect.X		 = 0.0f;
		m_ScissorRect.Y		 = 0.0f;
	}

	
	IBuffer* Renderer3D::GetCameraCB()
	{
		return m_CameraBuffer.Get();
	}


	IBuffer* Renderer3D::GetLightCB()
	{
		return m_LightBuffer.Get();
	}


	IBuffer* Renderer3D::GetTransformCB()
	{
		return m_TransformBuffer.Get();
	}
	
	
	IRenderPass* Renderer3D::GetRenderPass()
	{
		return m_RenderPass.Get();
	}
	

	const FrameStatistics& Renderer3D::GetFrameStatistics() const
	{
		return m_FrameInfo;
	}
}
