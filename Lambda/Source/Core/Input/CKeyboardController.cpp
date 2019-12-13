#include "LambdaPch.h"
#include "Core/Input/CKeyboardController.h"
#include "Core/Event/CEventDispatcher.h"
#include "Core/Event/CKeyEvent.h"
#include "Core/CEnvironment.h"

namespace Lambda
{
	//-------------------
	//CKeyboardController 
	//-------------------

	CKeyboardController::CKeyboardController(CEnvironment* pEnvironment)
		: IKeyboardController(),
		m_pEnvironment(pEnvironment),
		m_KeyboardState()
	{
	}


	CKeyboardController::~CKeyboardController()
	{
		m_pEnvironment->RemoveEventListener(this);
	}


	CKeyboardState CKeyboardController::GetKeyboardState() const
	{
		return m_KeyboardState;
	}
	
	
	bool CKeyboardController::OnEvent(const CEvent& event)
	{
		CEventDispatcher dispatcher;
		dispatcher.Dispatch(this, &CKeyboardController::OnKeyPressed, event);
		dispatcher.Dispatch(this, &CKeyboardController::OnKeyReleased, event);
		return false;
	}


	bool CKeyboardController::IsKeyUp(EKey key) const
	{
		return m_KeyboardState.IsKeyUp(key);
	}


	bool CKeyboardController::IsKeyDown(EKey key) const
	{
		return m_KeyboardState.IsKeyDown(key);
	}


	bool CKeyboardController::OnKeyPressed(const CKeyPressedEvent& event)
	{
		//LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Key '%d' pressed\n", event.GetKey());
		
		m_KeyboardState.SetKeyStateDown(event.GetKey());
		return false;
	}


	bool CKeyboardController::OnKeyReleased(const CKeyReleasedEvent& event)
	{
		//LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Key '%d' released\n", event.GetKey());

		m_KeyboardState.SetKeyStateUp(event.GetKey());
		return false;
	}
}