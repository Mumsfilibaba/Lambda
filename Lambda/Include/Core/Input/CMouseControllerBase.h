#pragma once
#include "IMouseController.h"
#include "../Event/IEventListener.h"

namespace Lambda
{
	class CMouseMovedEvent;
	class CMouseScrolledEvent;
	class CMouseButtonPressedEvent;
	class CMouseButtonReleasedEvent;

	//--------------------
	//CMouseControllerBase 
	//--------------------

	class CMouseControllerBase : public IMouseController, public IEventListener
	{
	public:
		CMouseControllerBase();
		virtual ~CMouseControllerBase();

		virtual bool OnEvent(const CEvent& event) override final;
		
		virtual bool IsMouseVisible() const override = 0;
		virtual void SetMouseVisisble(bool) override = 0;
		virtual void SetPosition(const Point& position) override = 0;
		virtual Point GetPosition() const override = 0;

		virtual bool IsButtonUp(EMouseButton button) const override final;
		virtual bool IsButtonDown(EMouseButton button) const override final;
		virtual CMouseState GetState() const override final;
	private:
		bool OnMouseScroll(const CMouseScrolledEvent& event);
		bool OnMouseButtonPressed(const CMouseButtonPressedEvent& event);
		bool OnMouseButtonReleased(const CMouseButtonReleasedEvent& event);
	private:
		CMouseState m_MouseState;
	};
}