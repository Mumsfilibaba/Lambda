#pragma once
#include "CKeyboardState.h"

namespace Lambda
{
    //---------
    //CKeyboard
    //---------
	class LAMBDA_API CKeyboard final
	{
		friend void Input::Update();
	public:
		LAMBDA_STATIC_CLASS(CKeyboard);

        // Difference between key-down and -pressed is that pressed only checks the current frame's state
		static bool IsKeyPressed(EKey key);
		static bool IsKeyUp(EKey key);
		static bool IsKeyDown(EKey key);
		static void GetState();
    private:
        static void UpdateState();
	private:
		static CKeyboardState s_CurrentState;
		static CKeyboardState s_LastState;
	};
}
