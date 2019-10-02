#pragma once
#include "Events/EventDispatcher.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"
#include "Graphics/Core/ITexture.h"
#include "Graphics/Core/IShader.h"
#include "Graphics/Core/ISamplerState.h"
#include "Graphics/Core/IPipelineState.h"
#include "Graphics/Core/IBuffer.h"
#include "System/Layer.h"
#include "Time/Timestep.h"
#include <imgui.h>

namespace Lambda
{
	class IBuffer;
	class IShader;
	class ITexture;
	class IRenderPass;
	class ISamplerState;
	class IDeviceContext;
	class IPipelineState;

	//-------
	//UILayer
	//-------

	class LAMBDA_API UILayer : public Layer
	{
	public:
		LAMBDA_NO_COPY(UILayer);

		UILayer();
		~UILayer() = default;

		void Init(IRenderPass* pRenderPass, IDeviceContext* pList);
        void Begin(Timestep time);
        void End();
        void Draw(IDeviceContext* pList);

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
		AutoRef<IShader>		m_VS;
		AutoRef<IShader>		m_PS;
		AutoRef<ISamplerState>	m_SamplerState;
		AutoRef<IPipelineState>	m_PipelineState;
		AutoRef<ITexture>		m_FontTexture;
		AutoRef<IBuffer>		m_VertexBuffer;
		AutoRef<IBuffer>		m_IndexBuffer;
	};
}
