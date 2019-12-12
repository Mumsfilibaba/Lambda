#include "LambdaPch.h"
#include "Core/Input/CEventMouseController.h"
#include "Core/Event/CEventDispatcher.h"
#include "Core/Event/CMouseEvent.h"
#include "Core/CEnvironment.h"

namespace Lambda
{
	//---------------------
	//CEventMouseController 
	//---------------------

	CEventMouseController::CEventMouseController()
		: IMouseController(),
		m_MouseState()
	{
	}

	
	CEventMouseController::~CEventMouseController()
	{
		CEnvironment& environment = CEnvironment::Get();
		environment.RemoveEventListener(this);
	}
	

	bool CEventMouseController::IsButtonup(EMouseButton button) const
	{
		return m_MouseState.IsButtonUp(button);
	}

	
	bool CEventMouseController::IsButtonDown(EMouseButton button) const
	{
		return m_MouseState.IsButtonDown(button);
	}


	CMouseState CEventMouseController::GetState() const
	{
		CMouseState mouseState = m_MouseState;
		mouseState.SetPosition(this->GetPosition());
		return mouseState;
	}


	bool CEventMouseController::OnEvent(const CEvent& event)
	{
		CEventDispatcher dispatcher;
		dispatcher.Dispatch(this, &CEventMouseController::OnMouseScroll, event);
		dispatcher.Dispatch(this, &CEventMouseController::OnMouseButtonPressed, event);
		dispatcher.Dispatch(this, &CEventMouseController::OnMouseButtonReleased, event);
		return false;
	}


	bool CEventMouseController::OnMouseScroll(const CMouseScrolledEvent& event)
	{
		//LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Mouse Scrolled (Hor:%.6f, Vert:%.6f)\n", event.GetHorizontalValue(), event.GetVerticalValue());

		m_MouseState.SetVerticalScollValue(event.GetVerticalValue());
		m_MouseState.SetHorizontalScollValue(event.GetHorizontalValue());
		return false;
	}


	bool CEventMouseController::OnMouseButtonPressed(const CMouseButtonPressedEvent& event)
	{
		//LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Mouse Button Pressed\n");

		m_MouseState.SetButtonDown(event.GetButton());
		return false;
	}


	bool CEventMouseController::OnMouseButtonReleased(const CMouseButtonReleasedEvent& event)
	{
		//LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Mouse Button Released\n");

		m_MouseState.SetButtonUp(event.GetButton());
		return false;
	}
}