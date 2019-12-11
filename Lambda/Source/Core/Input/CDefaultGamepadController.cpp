#include "LambdaPch.h"
#include "Core/Input/CDefaultGamepadController.h"

namespace Lambda
{
	//-------------------------
	//CDefaultGamepadController 
	//-------------------------

	CDefaultGamepadController::CDefaultGamepadController()
		: m_GamepadState()
	{
	}


	CGamepadState CDefaultGamepadController::GetGamepadState(EGamepad) const
	{
		//This is a dummy gampad controller so just return a state
		return m_GamepadState;
	}
}