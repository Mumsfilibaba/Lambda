#include "LambdaPch.h"
#include "Core/Input/CGamepad.h"
#include "Core/Input/IGamepadController.h"
#include "Core/CEngine.h"

#define GET_GAMEPADCONTROLLER Lambda::CEngine::Get().GetGamepadController

namespace Lambda
{
    //--------
    //CGamepad
    //--------

    CGamepadState CGamepad::s_LastState[4];
    CGamepadState CGamepad::s_CurrentState[4];

	float CGamepad::GetTrigger(EGamepad gamepadID, EGamepadTrigger trigger)
	{
		IGamepadController* pGamepadController = GET_GAMEPADCONTROLLER();
		return pGamepadController->GetTrigger(gamepadID, trigger);
	}


	const Vec2& CGamepad::GetThumbStick(EGamepad gamepadID, EGamepadStick stick)
	{
		IGamepadController* pGamepadController = GET_GAMEPADCONTROLLER();
		return pGamepadController->GetThumbStick(gamepadID, stick);
	}


	const CGamepadState& CGamepad::GetState(EGamepad gamepadID)
	{
		IGamepadController* pGamepadController = GET_GAMEPADCONTROLLER();
		s_CurrentState[gamepadID] = pGamepadController->GetGamepadState(gamepadID);
		return s_CurrentState[gamepadID];
	}


	void CGamepad::UpdateState()
    {
        IGamepadController* pGamepadController = GET_GAMEPADCONTROLLER();
        for (uint32 i = 0; i < GAMEPAD_LAST; i++)
        {
            s_LastState[i]    = s_CurrentState[i];
            s_CurrentState[i] = pGamepadController->GetGamepadState(EGamepad(i));
        }
    }
}
