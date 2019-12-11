#pragma once
#include "IMouseController.h"
#include "../Event/IEventListener.h"

namespace Lambda
{
	class CMouseMovedEvent;
	class CMouseScrolledEvent;
	class CMouseButtonPressedEvent;
	class CMouseButtonReleasedEvent;

	//-----------------------
	//CDefaultMouseController 
	//-----------------------

	class CDefaultMouseController final : public IMouseController, public IEventListener
	{
	public:
		CDefaultMouseController();
		~CDefaultMouseController() = default;

		virtual void SetPosition(const glm::ivec2& position) override final;
		virtual glm::ivec2 GetPosition() const override final;
		virtual CMouseState GetMouseState() const override final;
		virtual bool OnEvent(const CEvent& event) override final;
	private:
		bool OnMouseMove(const CMouseMovedEvent& event);
		bool OnMouseScroll(const CMouseScrolledEvent& event);
		bool OnMouseButtonPressed(const CMouseButtonPressedEvent& event);
		bool OnMouseButtonReleased(const CMouseButtonReleasedEvent& event);
	private:
		CMouseState m_MouseState;
	};
}