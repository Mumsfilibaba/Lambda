#include "LambdaPch.h"
#include "Core/Input/CDefaultKeyboardController.h"
#include "Core/Event/CEventDispatcher.h"
#include "Core/Event/CKeyEvent.h"

namespace Lambda
{
	//--------------------------
	//CDefaultKeyboardController 
	//--------------------------

	CDefaultKeyboardController::CDefaultKeyboardController()
		: m_KeyboardState()
	{
	}


	CKeyboardState CDefaultKeyboardController::GetKeyboardState() const
	{
		return m_KeyboardState;
	}
	
	
	bool CDefaultKeyboardController::OnEvent(const CEvent& event)
	{
		CEventDispatcher dispatcher;
		dispatcher.Dispatch(this, &CDefaultKeyboardController::OnKeyPressed, event);
		dispatcher.Dispatch(this, &CDefaultKeyboardController::OnKeyReleased, event);
		return false;
	}


	bool CDefaultKeyboardController::OnKeyPressed(const CKeyPressedEvent& event)
	{
		m_KeyboardState.SetKeyStateDown(event.GetKey());
		return false;
	}


	bool CDefaultKeyboardController::OnKeyReleased(const CKeyReleasedEvent& event)
	{
		m_KeyboardState.SetKeyStateUp(event.GetKey());
		return false;
	}
}