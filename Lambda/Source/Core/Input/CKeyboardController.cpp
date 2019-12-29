#include "LambdaPch.h"
#include "Core/Input/CKeyboardController.h"
#include "Core/Event/CEventDispatcher.h"
#include "Core/CEngine.h"

namespace Lambda
{
	//-------------------
	//CKeyboardController 
	//-------------------

	CKeyboardController::CKeyboardController()
		: IKeyboardController(),
		m_bKeyStates()
	{
		CEventDispatcher::GetPtr()->AddEventListener(this);
	}


	CKeyboardController::~CKeyboardController()
	{
		CEventDispatcher::GetPtr()->RemoveEventListener(this);
	}


	CKeyboardState CKeyboardController::GetKeyboardState() const
	{
		return CKeyboardState(m_bKeyStates);
	}
	
	
	bool CKeyboardController::OnEvent(const SSystemEvent& event)
	{
		if (event.EventType == ESystemEvent::SYSTEM_EVENT_KEY_PRESSED)
		{
			LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Key '%d' pressed\n", event.KeyEvent.Key);
			m_bKeyStates[(int32)event.KeyEvent.Key] = true;
		}
		else if (event.EventType == ESystemEvent::SYSTEM_EVENT_KEY_RELEASED)
		{
			LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Key '%d' released\n", event.KeyEvent.Key);
			m_bKeyStates[(int32)event.KeyEvent.Key] = false;
		}

		return false;
	}


	bool CKeyboardController::IsKeyUp(EKey key) const
	{
		return m_bKeyStates[(int32)key] == false;
	}


	bool CKeyboardController::IsKeyDown(EKey key) const
	{
		return m_bKeyStates[(int32)key] == true;
	}
}