#pragma once
#include "Events/EventDispatcher.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"

namespace Lambda
{
	//----------
	//ImGuiLayer
	//----------

	class LAMBDA_API ImGuiLayer : public EventLayer
	{
	public:
		LAMBDA_NO_COPY(ImGuiLayer);

		ImGuiLayer();
		~ImGuiLayer() = default;

        virtual void    OnPop() override final;
        virtual void    OnPush() override final;
        virtual bool    OnEvent(const Event& event) override final;
        virtual uint32  GetRecivableCategories() const override final;

		bool OnKeyTyped(const KeyTypedEvent& event);
		bool OnKeyPressed(const KeyPressedEvent& event);
		bool OnKeyReleased(const KeyReleasedEvent& event);
		bool OnMouseScroll(const MouseScrolledEvent& event);
		bool OnMousePressed(const MouseButtonPressedEvent& event);
		bool OnMouseReleased(const MouseButtonReleasedEvent& event);
		bool OnMouseMove(const MouseMovedEvent& event);
		bool OnWindowResize(const WindowResizeEvent& event);
	};
}
