#include "LambdaPch.h"
#include "Core/Input/CKeyboard.h"
#include "Core/Input/IKeyboardController.h"
#include "Core/CEnvironment.h"

#define GET_KEYBOARDCONTROLLER Lambda::CEnvironment::Get().GetKeyboardController

namespace Lambda
{
    //---------
    //CKeyboard
    //---------

    CKeyboardState CKeyboard::s_LastState		= CKeyboardState();
    CKeyboardState CKeyboard::s_CurrentState	= CKeyboardState();

	bool CKeyboard::IsKeyPressed(EKey key)
	{
		IKeyboardController* pKeyboardController = GET_KEYBOARDCONTROLLER();
		return s_LastState.IsKeyUp(key) && pKeyboardController->IsKeyDown(key);
	}


	bool CKeyboard::IsKeyUp(EKey key)
	{
		IKeyboardController* pKeyboardController = GET_KEYBOARDCONTROLLER();
		return pKeyboardController->IsKeyUp(key);
	}


	bool CKeyboard::IsKeyDown(EKey key)
	{
		IKeyboardController* pKeyboardController = GET_KEYBOARDCONTROLLER();
		return pKeyboardController->IsKeyDown(key);
	}


	const CKeyboardState& CKeyboard::GetState()
	{
		IKeyboardController* pKeyboardController = GET_KEYBOARDCONTROLLER();
		s_CurrentState = pKeyboardController->GetKeyboardState();
		return s_CurrentState;
	}


	void CKeyboard::Update()
    {
        s_LastState = s_CurrentState;

        IKeyboardController* pKeyboardController = GET_KEYBOARDCONTROLLER();
        s_CurrentState = pKeyboardController->GetKeyboardState();
    }
}
