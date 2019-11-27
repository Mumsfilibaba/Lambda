#include "LambdaPch.h"
#include "Graphics/Renderer3D.h"
#include "Graphics/Camera.h"
#include "Graphics/Core/IDevice.h"
#include "Graphics/Core/IBuffer.h"
#include "Graphics/Core/IQuery.h"
#include <glm/gtc/type_ptr.hpp>

namespace Lambda
{
	//----------
	//Renderer3D
	//----------

	Renderer3D::Renderer3D()
		: m_Device(nullptr),
        m_Context(nullptr),
        m_SwapChain(nullptr),
        m_Queries(),
		m_pCurrentQuery(nullptr),
		m_LightBuffer(nullptr),
		m_CameraBuffer(nullptr),
		m_TransformBuffer(nullptr),
		m_MaterialBuffer(nullptr),
		m_Viewport(),
		m_ScissorRect(),
		m_FrameInfo(),
		m_FrameClock(),
		m_CurrentFPS(0),
        m_QueryIndex(0)
	{
	}


	void Renderer3D::Init()
	{
		const SwapChainDesc& desc = m_SwapChain->GetDesc();

		for (uint32 i = 0; i < desc.BufferCount; i++)
		{
			//Create queries for timesteps
			IQuery* pQuery			= nullptr;
			QueryDesc queryDesc		= {};
			queryDesc.Type			= QUERY_TYPE_TIMESTAMP;
			queryDesc.QueryCount	= 2;
			m_Device->CreateQuery(&pQuery, queryDesc);
			m_Queries.emplace_back(pQuery);
		}

		//Get context
        m_Context = m_Device->GetImmediateContext();

		//Current query
        m_pCurrentQuery = m_Queries[0].Get();
        
		//Create camerabuffer
		BufferDesc cameraBufferdesc = {};
		cameraBufferdesc.pName			= "CameraBuffer";
		cameraBufferdesc.Usage			= USAGE_DEFAULT;
		cameraBufferdesc.Flags			= BUFFER_FLAGS_CONSTANT_BUFFER;
		cameraBufferdesc.SizeInBytes	= sizeof(CameraBuffer);
		cameraBufferdesc.StrideInBytes	= sizeof(CameraBuffer);
		m_Device->CreateBuffer(&m_CameraBuffer, nullptr, cameraBufferdesc);

		//Create lightbuffer
		BufferDesc lightBufferDesc = {};
		lightBufferDesc.pName			= "LightBuffer";
		lightBufferDesc.Usage			= USAGE_DEFAULT;
		lightBufferDesc.Flags			= BUFFER_FLAGS_CONSTANT_BUFFER;
		lightBufferDesc.SizeInBytes		= sizeof(LightBuffer);
		lightBufferDesc.StrideInBytes	= sizeof(LightBuffer);
		m_Device->CreateBuffer(&m_LightBuffer, nullptr, lightBufferDesc);

		//Create TransformBuffer
		BufferDesc transformBufferdesc = {};
		transformBufferdesc.pName			= "TransformBuffer";
		transformBufferdesc.Usage			= USAGE_DYNAMIC;
		transformBufferdesc.Flags			= BUFFER_FLAGS_CONSTANT_BUFFER;
		transformBufferdesc.SizeInBytes		= sizeof(TransformBuffer);
		transformBufferdesc.StrideInBytes	= sizeof(TransformBuffer);

		ResourceData data = {};
		data.pData			= glm::value_ptr(glm::mat4(1.0f));
		data.SizeInBytes	= transformBufferdesc.SizeInBytes;

		m_Device->CreateBuffer(&m_TransformBuffer, &data, transformBufferdesc);

		//Create materialbuffer
		BufferDesc materialBufferDesc = {};
		materialBufferDesc.pName			= "MaterialBuffer";
		materialBufferDesc.Usage			= USAGE_DYNAMIC;
		materialBufferDesc.Flags			= BUFFER_FLAGS_CONSTANT_BUFFER;
		materialBufferDesc.SizeInBytes		= sizeof(MaterialBuffer);
		materialBufferDesc.StrideInBytes	= sizeof(MaterialBuffer);
		m_Device->CreateBuffer(&m_MaterialBuffer, nullptr, materialBufferDesc);

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
	
		//Reset new query
		m_pCurrentQuery = m_Queries[m_QueryIndex].Get();
		m_Context->ResetQuery(m_pCurrentQuery);
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
		m_Context->UpdateBuffer(m_CameraBuffer.Get(), cameraData);

		//Update lightbuffer
		ResourceData lightData = {};
		lightData.pData			= &light;
		lightData.SizeInBytes	= sizeof(LightBuffer);
		m_Context->UpdateBuffer(m_LightBuffer.Get(), lightData);

		//Transition framebuffer
        ITexture* pRenderTarget = m_SwapChain->GetBuffer();
        ITexture* pDepthBuffer = m_SwapChain->GetDepthBuffer();
		
		TextureTransitionBarrier barriers[2];
		barriers[0].pTexture	= pRenderTarget;
		barriers[0].AfterState	= RESOURCE_STATE_RENDERTARGET_CLEAR;
		barriers[0].MipLevel	= LAMBDA_ALL_MIP_LEVELS;
		barriers[1].pTexture	= pDepthBuffer;
		barriers[1].AfterState	= RESOURCE_STATE_DEPTH_STENCIL_CLEAR;
		barriers[1].MipLevel	= LAMBDA_ALL_MIP_LEVELS;
		m_Context->TransitionTextureStates(barriers, 2);

        //Clear framebuffer
        float color[] = { 0.392f, 0.584f, 0.929f, 1.0f };
        m_Context->ClearRenderTarget(pRenderTarget, color);
        m_Context->ClearDepthStencil(pDepthBuffer, 1.0f, 0);
        
		//Transition into rendertarget and depthstencil
		barriers[0].AfterState = RESOURCE_STATE_RENDERTARGET;
		barriers[1].AfterState = RESOURCE_STATE_DEPTH_STENCIL;
		m_Context->TransitionTextureStates(barriers, 2);

		//Set rendertargets
		m_Context->SetRendertargets(&pRenderTarget, 1, pDepthBuffer);

		//Write query
		m_Context->WriteTimeStamp(m_pCurrentQuery);
		//Set viewport
		m_Context->SetViewports(&m_Viewport, 1);
		m_Context->SetScissorRects(&m_ScissorRect, 1);
	}


	void Renderer3D::Submit(const Model& model, const Material& material, const TransformBuffer& transform)
	{
		//Update transform
		void* pTransformData = nullptr;
		m_Context->MapBuffer(m_TransformBuffer.Get(), MAP_FLAG_WRITE | MAP_FLAG_WRITE_DISCARD, &pTransformData);
		memcpy(pTransformData, &transform, sizeof(TransformBuffer));
		m_Context->UnmapBuffer(m_TransformBuffer.Get());

		//Update material
		MaterialBuffer materialBuffer = {};
		materialBuffer.Color		= material.Color;
		materialBuffer.HasAlbedoMap = material.HasAlbedoMap;
		materialBuffer.HasNormalMap = material.HasNormalMap;
        
		void* pMaterialData = nullptr;
		m_Context->MapBuffer(m_MaterialBuffer.Get(), MAP_FLAG_WRITE | MAP_FLAG_WRITE_DISCARD, &pMaterialData);
		memcpy(pMaterialData, &materialBuffer, sizeof(MaterialBuffer));
		m_Context->UnmapBuffer(m_MaterialBuffer.Get());

		//Set mesh
        m_Context->SetVertexBuffers(&model.pVertexBuffer, 1, 0);
        m_Context->SetIndexBuffer(model.pIndexBuffer, FORMAT_R32_UINT);
        //Set pipelinestate
		m_Context->SetShaderVariableTable(material.pVariableTable);
		m_Context->SetPipelineState(material.pPipelineState);
        //Draw
        m_Context->DrawIndexedInstanced(model.IndexCount, 1, 0, 0, 0);
	}


	void Renderer3D::EndScene() const
	{
		//End by writing a timestamp
		m_Context->WriteTimeStamp(m_pCurrentQuery);
	}


	void Renderer3D::End()
	{
		//Get last frame's values from query
		uint64 values[2] = { 0, 0 };
		m_pCurrentQuery->GetResults(values, 2, 0);
		m_FrameInfo.GPUTime = Timestep(values[1] - values[0]);
	}


	void Renderer3D::Swapbuffers()
	{
		//Present
		m_SwapChain->Present();
        m_QueryIndex = (m_QueryIndex+1) % m_SwapChain->GetDesc().BufferCount;
	}


	void Renderer3D::Release()
	{
        //Release queries
        for (auto& query : m_Queries)
            query.Release();
        m_Queries.clear();
        
		//Destroy camerabuffer
        m_CameraBuffer.Release();
        
		//Destroy lightbuffer
        m_LightBuffer.Release();
        
		//Destroy transformbuffer
        m_TransformBuffer.Release();
        
		//Destroy materialbuffer
        m_MaterialBuffer.Release();

        //Release context, device and swapchain
        m_SwapChain.Release();
        m_Context.Release();
        m_Device.Release();
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

		//Resize
		m_SwapChain->ResizeBuffers(width, height);
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
	

	const FrameStatistics& Renderer3D::GetFrameStatistics() const
	{
		return m_FrameInfo;
	}
}
