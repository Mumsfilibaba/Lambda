#pragma once
#include "LambdaCore.h"
#include "CKeyboardState.h"

namespace Lambda
{
    //---------
    //CKeyboard
    //---------

	class LAMBDA_API CKeyboard final
	{
        friend class CEngine;
	public:
		LAMBDA_STATIC_CLASS(Keyboard);

		static bool IsKeyDown(EKey key);
		static bool IsKeyUp(EKey key);
        // Difference between key-down and -pressed is that pressed only checks the current frame's state
		static bool IsKeyPressed(EKey key);
		static const KeyboardState& GetState();
    private:
        static void Update();
	private:
		static KeyboardState s_CurrentState;
		static KeyboardState s_LastState;
	};
}
