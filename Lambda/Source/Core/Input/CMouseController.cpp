#include "LambdaPch.h"
#include "Core/Input/CMouseController.h"
#include "Core/Event/CEventDispatcher.h"

namespace Lambda
{
	//----------------
	//CMouseController 
	//----------------

	CMouseController::CMouseController()
		: IMouseController(),
		m_MouseState(),
		m_bIsMouseVisible(true)
	{
		CEventDispatcher::GetPtr()->AddEventListener(this);
	}

	
	CMouseController::~CMouseController()
	{
		CEventDispatcher::GetPtr()->RemoveEventListener(this);
	}
	

	bool CMouseController::IsButtonUp(EMouseButton button) const
	{
		return m_MouseState.IsButtonUp(button);
	}

	
	bool CMouseController::IsButtonDown(EMouseButton button) const
	{
		return m_MouseState.IsButtonDown(button);
	}


	bool CMouseController::IsMouseVisible() const
	{
		return m_bIsMouseVisible;
	}


	void CMouseController::SetMouseVisisble(bool bVisible)
	{
		if (m_bIsMouseVisible != bVisible)
		{
			m_bIsMouseVisible = bVisible;
			LShowCursor(bVisible);
		}
	}


	void CMouseController::SetPosition(const Point& position)
	{
		LSetCursorPosition(position.x, position.y);
	}


	Point CMouseController::GetPosition() const
	{
		Point cursor = {};
		LGetCursorPosition(cursor.x, cursor.y);

		return cursor;
	}


	CMouseState CMouseController::GetState() const
	{
		Point cursor = {};
		LGetCursorPosition(cursor.x, cursor.y);
		
		CMouseState mouseState = m_MouseState;
		mouseState.SetPosition(cursor);

		return mouseState;
	}


	bool CMouseController::OnEvent(const CEvent& event)
	{
		LForwardEvent(this, &CMouseController::OnMouseScroll, event);
		LForwardEvent(this, &CMouseController::OnMouseButtonPressed, event);
		LForwardEvent(this, &CMouseController::OnMouseButtonReleased, event);
		return false;
	}


	bool CMouseController::OnMouseScroll(const CMouseScrolledEvent& event)
	{
		//LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Mouse Scrolled (Hor:%.6f, Vert:%.6f)\n", event.GetHorizontalValue(), event.GetVerticalValue());

		m_MouseState.SetVerticalScollValue(event.GetVerticalValue());
		m_MouseState.SetHorizontalScollValue(event.GetHorizontalValue());
		return false;
	}


	bool CMouseController::OnMouseButtonPressed(const CMouseButtonPressedEvent& event)
	{
		//LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Mouse Button Pressed\n");

		m_MouseState.SetButtonDown(event.GetButton());
		return false;
	}


	bool CMouseController::OnMouseButtonReleased(const CMouseButtonReleasedEvent& event)
	{
		//LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Mouse Button Released\n");

		m_MouseState.SetButtonUp(event.GetButton());
		return false;
	}
}