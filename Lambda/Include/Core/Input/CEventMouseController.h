#pragma once
#include "IMouseController.h"
#include "../Event/IEventListener.h"

namespace Lambda
{
	class CMouseMovedEvent;
	class CMouseScrolledEvent;
	class CMouseButtonPressedEvent;
	class CMouseButtonReleasedEvent;

	//---------------------
	//CEventMouseController 
	//---------------------

	class CEventMouseController final : public IMouseController, public IEventListener
	{
	public:
		CEventMouseController();
		~CEventMouseController() = default;

		virtual bool OnEvent(const CEvent& event) override final;
		virtual bool IsButtonup(EMouseButton button) const override final;
		virtual bool IsButtonDown(EMouseButton button) const override final;
		virtual void SetPosition(const Point& position) override final;
		virtual Point GetPosition() const override final;
		virtual CMouseState GetState() const override final;
	private:
		bool OnMouseMove(const CMouseMovedEvent& event);
		bool OnMouseScroll(const CMouseScrolledEvent& event);
		bool OnMouseButtonPressed(const CMouseButtonPressedEvent& event);
		bool OnMouseButtonReleased(const CMouseButtonReleasedEvent& event);
	private:
		CMouseState m_MouseState;
	};
}