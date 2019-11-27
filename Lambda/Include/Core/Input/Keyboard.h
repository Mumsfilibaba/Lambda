#pragma once
#include "LambdaCore.h"
#include "KeyboardState.h"

namespace Lambda
{
	class LAMBDA_API Keyboard final
	{
	public:
		LAMBDA_STATIC_CLASS(Keyboard);

		static bool IsKeyDown(Key key);
		static bool IsKeyUp(Key key);
		static bool IsKeyPressed(Key key);
		static const KeyboardState& GetState();
	private:
		static KeyboardState s_CurrentState;
		static KeyboardState s_LastState;
	};
}