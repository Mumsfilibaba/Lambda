#include "LambdaPch.h"
#include "Core/Input/CEventKeyboardController.h"
#include "Core/Event/CEventDispatcher.h"
#include "Core/Event/CKeyEvent.h"

namespace Lambda
{
	//------------------------
	//CEventKeyboardController 
	//------------------------

	CEventKeyboardController::CEventKeyboardController()
		: m_KeyboardState()
	{
	}


	CKeyboardState CEventKeyboardController::GetKeyboardState() const
	{
		return m_KeyboardState;
	}
	
	
	bool CEventKeyboardController::OnEvent(const CEvent& event)
	{
		CEventDispatcher dispatcher;
		dispatcher.Dispatch(this, &CEventKeyboardController::OnKeyPressed, event);
		dispatcher.Dispatch(this, &CEventKeyboardController::OnKeyReleased, event);
		return false;
	}


	bool CEventKeyboardController::IsKeyUp(EKey key) const
	{
		return m_KeyboardState.IsKeyUp(key);
	}


	bool CEventKeyboardController::IsKeyDown(EKey key) const
	{
		return m_KeyboardState.IsKeyDown(key);
	}


	bool CEventKeyboardController::OnKeyPressed(const CKeyPressedEvent& event)
	{
		//LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Key pressed\n");
		
		m_KeyboardState.SetKeyStateDown(event.GetKey());
		return false;
	}


	bool CEventKeyboardController::OnKeyReleased(const CKeyReleasedEvent& event)
	{
		//LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Key released\n");

		m_KeyboardState.SetKeyStateUp(event.GetKey());
		return false;
	}
}