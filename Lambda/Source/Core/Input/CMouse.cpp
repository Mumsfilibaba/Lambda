#include "LambdaPch.h"
#include "Core/Input/CMouse.h"
#include "Core/Input/IMouseController.h"
#include "Core/CEnvironment.h"

namespace Lambda
{
    //------
    //CMouse
    //------

    CMouseState CMouse::s_LastState = CMouseState();
    CMouseState CMouse::s_CurrentState = CMouseState();

    void CMouse::Update()
    {
        CEnvironment& environment = CEnvironment::Get();
        IMouseController* pMouseController = environment.GetMouseController();
        
        s_LastState = s_CurrentState;
        s_CurrentState = pMouseController->GetMouseState();
    }
}
