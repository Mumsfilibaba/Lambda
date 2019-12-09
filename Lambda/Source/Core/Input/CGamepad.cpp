#include "LambdaPch.h"
#include "Core/Input/CGamepad.h"
#include "Core/Input/IGamepadController.h"
#include "Core/CEnvironment.h"

namespace Lambda
{
    //--------
    //CGamepad
    //--------

    CGamepadState CGamepad::s_LastState;
    CGamepadState CGamepad::s_CurrentState;

    void CGamepad::Update()
    {
        CEnvironment& environment = CEnvironment::Get();
        IGamepadController* pGamepadController = environment.GetGamepadController();

        for (uint32 i = 0; i < GAMEPAD_LAST; i++)
        {
            s_LastState[i]      = s_CurrentState[i];
            s_CurrentState[i]   = pGamepadController->GetGamepadState();
        }
        
    }
}
