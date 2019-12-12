#pragma once
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
		LAMBDA_STATIC_CLASS(CKeyboard);

        // Difference between key-down and -pressed is that pressed only checks the current frame's state
		static bool IsKeyPressed(EKey key);
		static bool IsKeyUp(EKey key);
		static bool IsKeyDown(EKey key);
		static const CKeyboardState& GetState();
    private:
        static void UpdateState();
	private:
		static CKeyboardState s_CurrentState;
		static CKeyboardState s_LastState;
	};
}
