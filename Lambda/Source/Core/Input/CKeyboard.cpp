#include "LambdaPch.h"
#include "Core/Input/CKeyboard.h"
#include "Core/Input/IKeyboardController.h"
#include "Core/CEnvironment.h"

namespace Lambda
{
    //---------
    //CKeyboard
    //---------

    CKeyboardState CKeyboard::s_LastState		= CKeyboardState();
    CKeyboardState CKeyboard::s_CurrentState	= CKeyboardState();

	bool CKeyboard::IsKeyPressed(EKey key)
	{
		return s_LastState.IsKeyUp(key) && s_CurrentState.IsKeyDown(key);
	}


	bool CKeyboard::IsKeyUp(EKey key)
	{
		return s_LastState.IsKeyUp(key);
	}


	bool CKeyboard::IsKeyDown(EKey key)
	{
		return s_CurrentState.IsKeyDown(key);
	}


	const CKeyboardState& CKeyboard::GetState()
	{
		return s_CurrentState;
	}


	void CKeyboard::Update()
    {
        CEnvironment& environment = CEnvironment::Get();
        IKeyboardController* pKeyboardController = environment.GetKeyboardController();

        s_LastState = s_CurrentState;
        s_CurrentState = pKeyboardController->GetKeyboardState();
    }
}
