#pragma once
#include "IMouseController.h"
#include "../Event/IEventListener.h"

namespace Lambda
{
	class CEnvironment;
	class CMouseMovedEvent;
	class CMouseScrolledEvent;
	class CMouseButtonPressedEvent;
	class CMouseButtonReleasedEvent;

	//----------------
	//CMouseController 
	//----------------

	class CMouseController : public IMouseController, public IEventListener
	{
	public:
		CMouseController();
		~CMouseController();

		virtual bool OnEvent(const SEvent& event) override final;
		
		virtual bool IsMouseVisible() const override final;
		virtual void SetMouseVisisble(bool bVisible) override final;
		virtual void SetPosition(const Point& position) override final;
		virtual Point GetPosition() const override final;

		virtual bool IsButtonUp(EMouseButton button) const override final;
		virtual bool IsButtonDown(EMouseButton button) const override final;
		virtual CMouseState GetState() const override final;
	private:
		bool OnMouseScroll(const CMouseScrolledEvent& event);
		bool OnMouseButtonPressed(const CMouseButtonPressedEvent& event);
		bool OnMouseButtonReleased(const CMouseButtonReleasedEvent& event);
	protected:
		CMouseState m_MouseState;
		bool m_bIsMouseVisible;
	};
}