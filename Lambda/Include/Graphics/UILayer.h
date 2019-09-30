#pragma once
#include "Events/EventDispatcher.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"
#include "Graphics/ITexture.h"
#include "Graphics/IShader.h"
#include "Graphics/ISamplerState.h"
#include "Graphics/IPipelineResourceState.h"
#include "Graphics/IPipelineState.h"
#include "Graphics/IBuffer.h"
#include "System/Layer.h"
#include "Time/Timestep.h"
#include <imgui.h>

namespace Lambda
{
	class IBuffer;
	class IShader;
	class ITexture;
	class IRenderPass;
	class ICommandList;
	class ISamplerState;
	class IGraphicsPipelineState;
	class IPipelineResourceState;

	//-------
	//UILayer
	//-------

	class LAMBDA_API UILayer : public Layer
	{
	public:
		LAMBDA_NO_COPY(UILayer);

		UILayer();
		~UILayer() = default;

		void Init(IRenderPass* pRenderPass, ICommandList* pList);
        void Begin(Timestep time);
        void End();
        void Draw(ICommandList* pList);

        virtual void OnLoad() override final;
        virtual void OnRenderUI(Timestep dt) override final;
        virtual void OnRelease() override final;
        virtual bool OnEvent(const Event& event) override final;
        virtual uint32 GetRecivableCategories() const override final;
    private:
		bool OnKeyTyped(const KeyTypedEvent& event);
		bool OnKeyPressed(const KeyPressedEvent& event);
		bool OnKeyReleased(const KeyReleasedEvent& event);
		bool OnMouseScroll(const MouseScrolledEvent& event);
		bool OnMousePressed(const MouseButtonPressedEvent& event);
		bool OnMouseReleased(const MouseButtonReleasedEvent& event);
		bool OnMouseMove(const MouseMovedEvent& event);
		bool OnWindowResize(const WindowResizeEvent& event);
        void Create();
	private:
		AutoRef<IShader>				m_VS;
		AutoRef<IShader>				m_PS;
		AutoRef<ISamplerState>			m_SamplerState;
		AutoRef<IPipelineResourceState> m_PipelineResourceState;
		AutoRef<IGraphicsPipelineState>	m_PipelineState;
		AutoRef<ITexture>				m_FontTexture;
		AutoRef<IBuffer>				m_VertexBuffer;
		AutoRef<IBuffer>				m_IndexBuffer;
	};
}
