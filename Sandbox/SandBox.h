#pragma once
#include "System/Application.h"
#include "Graphics/ICommandList.h"
#include "Graphics/IShader.h"
#include "Graphics/IPipelineState.h"
#include "Graphics/IBuffer.h"
#include "Graphics/ITexture2D.h"
#include "Graphics/ISamplerState.h"
#include "Graphics/IRenderPass.h"
#include "Graphics/IResourceState.h"
#include "Graphics/Camera.h"

namespace Lambda
{
    //Buffer for camera
	struct CameraBuffer
	{
        glm::mat4 View;
        glm::mat4 Proj;
	};
    
    
    //Buffer for transform
    struct TransformBuffer
    {
        glm::mat4 Model;
    };


    //Sandbox application
	class SandBox final : public Application
	{
	public:
		SandBox(const EngineParams& params = EngineParams());
		~SandBox();

		virtual void OnLoad() override final;
		virtual void OnUpdate(Time dt) override final;
		virtual void OnRender(Time dt) override final;
		virtual void OnRelease() override final;

		void CreateCamera(uint32 width, uint32 height);
        bool EventHandler(const Event& event);

	private:
        //Commandlist
		ICommandList* m_pLists[3];
        ICommandList* m_pCurrentList;
		
        //Shaders
        IShader* m_pVS;
		IShader* m_pPS;
        
        //Buffers
		IBuffer* m_pVertexBuffer;
        IBuffer* m_pIndexBuffer;
		IBuffer* m_pColorBuffer;
		IBuffer* m_pCameraBuffer;
        IBuffer* m_pTransformBuffer;
        
        //Textures
		ITexture2D* m_pTexture;
		
        //Samplers
        ISamplerState* m_pSamplerState;
		
        //Pipelinestates
        IGraphicsPipelineState* m_pPipelineState;

		//RenderPass
		IRenderPass* m_pRenderPass;
        
        //ResourceState
        IResourceState* m_pResourceState;
		
        //Other
        float m_Width;
		float m_Height;
		uint32 m_IndexCount;
        Camera m_Camera;
		CameraBuffer m_CameraBuffer;
        TransformBuffer m_TransformBuffer;

	public:
		static bool OnEvent(const Event& event);
	};
}
