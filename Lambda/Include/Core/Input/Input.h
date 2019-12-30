#pragma once
#include "LambdaCore.h"

#include "EKey.h"
#include "EMouse.h"

namespace Lambda
{
	class CMouseState;
	class CGamepadState;
	class CKeyboardState;

	//------
	//IInput
	//------
	class LAMBDA_API IInput
	{
	public:
		LAMBDA_INTERFACE(IInput);

		virtual bool IsKeyUp(EKey key) const = 0; 
		virtual bool IsKeyDown(EKey key) const = 0;

		virtual bool IsMouseButtonUp(EMouseButton button) const = 0;
		virtual bool IsMouseButtonDown(EMouseButton button) const = 0;

		virtual void SetMousePosition(uint32 x, uint32 y) = 0;
		virtual void GetMousePosition(uint32& outX, uint32& outY) = 0;

		virtual void GetKeyboardState(CMouseState& mouseState) const = 0;
		virtual void GetKeyboardState(CGamepadState& gamepadState) const = 0;
		virtual void GetKeyboardState(CKeyboardState& keyboardState) const = 0;

		virtual void Release() = 0;
	public:
		static IInput* Create();
	};
}