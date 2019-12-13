#include "LambdaPch.h"
#include "Core/Input/CMouseControllerBase.h"
#include "Core/Event/CEventDispatcher.h"
#include "Core/Event/CMouseEvent.h"
#include "Core/CEnvironment.h"

namespace Lambda
{
	//---------------------
	//CMouseControllerBase 
	//---------------------

	CMouseControllerBase::CMouseControllerBase()
		: IMouseController(),
		m_MouseState()
	{
	}

	
	CMouseControllerBase::~CMouseControllerBase()
	{
		CEnvironment& environment = CEnvironment::Get();
		environment.RemoveEventListener(this);
	}
	

	bool CMouseControllerBase::IsButtonUp(EMouseButton button) const
	{
		return m_MouseState.IsButtonUp(button);
	}

	
	bool CMouseControllerBase::IsButtonDown(EMouseButton button) const
	{
		return m_MouseState.IsButtonDown(button);
	}


	CMouseState CMouseControllerBase::GetState() const
	{
		CMouseState mouseState = m_MouseState;
		mouseState.SetPosition(this->GetPosition());
		return mouseState;
	}


	bool CMouseControllerBase::OnEvent(const CEvent& event)
	{
		CEventDispatcher dispatcher;
		dispatcher.Dispatch(this, &CMouseControllerBase::OnMouseScroll, event);
		dispatcher.Dispatch(this, &CMouseControllerBase::OnMouseButtonPressed, event);
		dispatcher.Dispatch(this, &CMouseControllerBase::OnMouseButtonReleased, event);
		return false;
	}


	bool CMouseControllerBase::OnMouseScroll(const CMouseScrolledEvent& event)
	{
		LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Mouse Scrolled (Hor:%.6f, Vert:%.6f)\n", event.GetHorizontalValue(), event.GetVerticalValue());

		m_MouseState.SetVerticalScollValue(event.GetVerticalValue());
		m_MouseState.SetHorizontalScollValue(event.GetHorizontalValue());
		return false;
	}


	bool CMouseControllerBase::OnMouseButtonPressed(const CMouseButtonPressedEvent& event)
	{
		//LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Mouse Button Pressed\n");

		m_MouseState.SetButtonDown(event.GetButton());
		return false;
	}


	bool CMouseControllerBase::OnMouseButtonReleased(const CMouseButtonReleasedEvent& event)
	{
		//LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Mouse Button Released\n");

		m_MouseState.SetButtonUp(event.GetButton());
		return false;
	}
}