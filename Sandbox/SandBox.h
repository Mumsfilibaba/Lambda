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
        virtual void OnRender(const Renderer3D& renderer, Timestep dt) override final;
        virtual void OnRenderUI(Timestep dt) override final;
        virtual void OnRelease() override final;
        virtual bool OnEvent(const Event& event) override final;
        virtual uint32 GetRecivableCategories() const override final;
    private:
        bool OnWindowResize(const WindowResizeEvent& event);
        bool OnKeyPressed(const KeyPressedEvent& event);
        void CreateCamera(uint32 width, uint32 height);
    private:
        //Shaders
        IShader* m_pVS;
        IShader* m_pPS;
        //Meshes
		Model m_Mesh;
		Model m_SphereMesh;
		//Material
		Material m_Material;
		Material m_RedMaterial;
        //Textures
        ITexture* m_pAlbedoMap;
        ITexture* m_pNormalMap;
        //Samplers
        ISamplerState* m_pSamplerState;
        //Pipelinestates
        IGraphicsPipelineState* m_pPipelineState;
        //ResourceState
        IPipelineResourceState* m_pResourceState;
        Camera m_Camera;
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
