#pragma once
#include "LambdaCore.h"
#include "CGamepadState.h"

namespace Lambda
{
    class CMouseState;
    class CKeyboardState;
    class IMouseController;
    class IGamepadController;
    class IKeyboardController;
    
    //-----
    //Input
    //-----
    namespace Input
    {
        bool LAMBDA_API Initialize();
        void LAMBDA_API Update();
        void LAMBDA_API Release();

        void LAMBDA_API SetMouseController(IMouseController* pMouseController);
        void LAMBDA_API SetGamepadController(IGamepadController* pGamepadController);
        void LAMBDA_API SetKeyboardController(IKeyboardController* pKeyboardController);

        void LAMBDA_API GetMouseState(CMouseState* pMouseState);
        void LAMBDA_API GetGamepadState(CGamepadState* pGamepadState);
        void LAMBDA_API GetKeyboardState(CKeyboardState* pKeyboardState);

        IMouseController* LAMBDA_API GetMouseController();
        IGamepadController* LAMBDA_API GetGamepadController();
        IKeyboardController* LAMBDA_API GetKeyboardController();
    }
}