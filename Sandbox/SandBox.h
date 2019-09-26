#pragma once
#include "System/Application.h"
#include "Graphics/ICommandList.h"
#include "Graphics/IShader.h"
#include "Graphics/IPipelineState.h"
#include "Graphics/IBuffer.h"
#include "Graphics/ITexture.h"
#include "Graphics/ISamplerState.h"
#include "Graphics/IRenderPass.h"
#include "Graphics/IPipelineResourceState.h"
#include "Graphics/IQuery.h"
#include "Graphics/Camera.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

namespace Lambda
{
	struct CameraBuffer
	{
        glm::mat4 View;
        glm::mat4 Projection;
        glm::vec3 Position;
	};
    
    
    struct TransformBuffer
    {
        glm::mat4 Model;
    };

    
    struct LightBuffer
    {
        glm::vec4 Color;
        glm::vec3 Position;
    };

    //------------
    //SandBoxLayer
    //------------

    class SandBoxLayer : public Layer
    {
    public:
        LAMBDA_NO_COPY(SandBoxLayer);

        SandBoxLayer();
        ~SandBoxLayer() = default;

        virtual void OnLoad() override final;
        virtual void OnUpdate(Timestep dt) override final;
        virtual void OnRender(Timestep dt, ICommandList* pCurrentList) override final;
        virtual void OnRenderUI(Timestep dt) override final;
        virtual void OnRelease() override final;
        virtual bool OnEvent(const Event& event) override final;
        virtual uint32 GetRecivableCategories() const override final;
    private:
        bool OnWindowResize(const WindowResizeEvent& event);
        bool OnKeyPressed(const KeyPressedEvent& event);
        void CreateCamera(uint32 width, uint32 height);
    private:
        //Querys
        IQuery* m_pQueries[3];
        //Shaders
        IShader* m_pVS;
        IShader* m_pPS;
        //Buffers
        IBuffer* m_pVertexBuffer;
        IBuffer* m_pIndexBuffer;
        IBuffer* m_pColorBuffer;
        IBuffer* m_pLightBuffer;
        IBuffer* m_pCameraBuffer;
        IBuffer* m_pTransformBuffer;
        //Textures
        ITexture* m_pAlbedo;
        ITexture* m_pNormal;
        //Samplers
        ISamplerState* m_pSamplerState;
        //Pipelinestates
        IGraphicsPipelineState* m_pPipelineState;
        //ResourceState
        IPipelineResourceState* m_pResourceState;
        //Other
        float m_Width;
        float m_Height;
        uint32 m_IndexCount;
        Camera m_Camera;
        CameraBuffer m_CameraBuffer;
        TransformBuffer m_TransformBuffer;
    };

    //-------
    //SandBox
    //-------

	class SandBox final : public Application
	{
	public:
		SandBox(const EngineParams& params = EngineParams());
		~SandBox() = default;
	};
}
