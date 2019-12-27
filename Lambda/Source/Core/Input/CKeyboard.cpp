#include "LambdaPch.h"
#include "Core/Input/CKeyboard.h"
#include "Core/Input/IKeyboardController.h"
#include "Core/CEngine.h"

namespace Lambda
{
    //---------
    //CKeyboard
    //---------

    CKeyboardState CKeyboard::s_LastState	 = CKeyboardState();
    CKeyboardState CKeyboard::s_CurrentState = CKeyboardState();

	bool CKeyboard::IsKeyPressed(EKey key)
	{
		IKeyboardController* pKeyboardController = CEngine::Get().GetKeyboardController();
		return s_LastState.IsKeyUp(key) && pKeyboardController->IsKeyDown(key);
	}


	bool CKeyboard::IsKeyUp(EKey key)
	{
		IKeyboardController* pKeyboardController = CEngine::Get().GetKeyboardController();
		return pKeyboardController->IsKeyUp(key);
	}


	bool CKeyboard::IsKeyDown(EKey key)
	{
		IKeyboardController* pKeyboardController = CEngine::Get().GetKeyboardController();
		return pKeyboardController->IsKeyDown(key);
	}


	const CKeyboardState& CKeyboard::GetState()
	{
		IKeyboardController* pKeyboardController = CEngine::Get().GetKeyboardController();
		s_CurrentState = pKeyboardController->GetKeyboardState();

		return s_CurrentState;
	}


	void CKeyboard::UpdateState()
    {
        s_LastState = s_CurrentState;

        IKeyboardController* pKeyboardController = CEngine::Get().GetKeyboardController();
        s_CurrentState = pKeyboardController->GetKeyboardState();
    }
}
