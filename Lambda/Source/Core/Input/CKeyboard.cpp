#include "LambdaPch.h"
#include "Core/Input/CKeyboard.h"
#include "Core/Input/IKeyboardController.h"
#include "Core/CEnvironment.h"

namespace Lambda
{
    //---------
    //CKeyboard
    //---------

    CKeyboardState CKeyboard::s_LastState = CKeyboardState();
    CKeyboardState CKeyboard::s_CurrentState = CKeyboardState();

    void CKeyboard::Update()
    {
        CEnvironment& environment = CEnvironment::Get();
        IKeyboardController* pKeyboardController = environment.GetKeyboardController();

        s_LastState = s_CurrentState;
        s_CurrentState = pKeyboardController->GetKeyboardState();
    }
}
