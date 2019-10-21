#pragma once
#include "System/Application.h"
#include "Graphics/Core/IDeviceContext.h"
#include "Graphics/Core/IShader.h"
#include "Graphics/Core/IPipelineState.h"
#include "Graphics/Core/IBuffer.h"
#include "Graphics/Core/ITexture.h"
#include "Graphics/Core/ISamplerState.h"
#include "Graphics/Core/IQuery.h"
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
        virtual void OnRender(Renderer3D& renderer, Timestep dt) override final;
        virtual void OnRenderUI(Timestep dt) override final;
        virtual void OnRelease() override final;
        virtual bool OnEvent(const Event& event) override final;
        virtual uint32 GetRecivableCategories() const override final;
    private:
        bool OnWindowResize(const WindowResizeEvent& event);
        bool OnKeyPressed(const KeyPressedEvent& event);
        void CreateCamera(uint32 width, uint32 height);
    private:
		//Context
		AutoRef<ISwapChain> m_SwapChain;
		AutoRef<IDeviceContext> m_Context;
        //Shaders
        AutoRef<IShader> m_VS;
        AutoRef<IShader> m_PS;
        //Pipelinestates
		AutoRef<IPipelineState> m_PipelineState;
		//Variable tables
		AutoRef<IShaderVariableTable> m_VariableTable;
        //Meshes
		Model m_Mesh;
		Viewport  m_Viewport;
		Rectangle m_ScissorRect;
		//Model m_SphereMesh;
		//Material
		//Material m_Material;
		//Material m_RedMaterial;
        //Textures
		//AutoRef<ITexture> m_AlbedoMap;
		//AutoRef<ITexture> m_NormalMap;
        //Samplers
		//AutoRef<ISamplerState> m_SamplerState;
        //Camera m_Camera;
        //TransformBuffer m_TransformBuffer;
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
