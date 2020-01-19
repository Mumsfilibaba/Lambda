#pragma once
#include "LambdaCore.h"

#include "EMouse.h"
#include "EKeyboard.h"

namespace Lambda
{
	class MouseState;
	class GamepadState;
	class KeyboardState;

	//----------
	//EInputType
	//----------
	
	enum class EInputType
	{
		INPUT_TYPE_UNKNOWN = 0,
		INPUT_TYPE_DEFAULT = 1,
	};

	//----------------
	//IInputController
	//----------------

	class IInputController
	{
		friend class Input;
	public:
		LAMBDA_INTERFACE(IInputController);

		virtual bool IsKeyUp(EKey key) const = 0; 
		virtual bool IsKeyDown(EKey key) const = 0;

		virtual bool IsMouseButtonUp(EMouseButton button) const = 0;
		virtual bool IsMouseButtonDown(EMouseButton button) const = 0;

		virtual void SetMousePosition(uint32 x, uint32 y) = 0;
		virtual void GetMousePosition(uint32& outX, uint32& outY) = 0;

		virtual void GetMouseState(MouseState& outMouseState) const = 0;
		virtual void GetGamepadState(GamepadState& outGamepadState) const = 0;
		virtual void GetKeyboardState(KeyboardState& outKeyboardState) const = 0;
	};

	//-----
	//Input
	//-----

	class LAMBDA_API Input
	{
	public:
		LAMBDA_STATIC_CLASS(Input);

		static void Init(EInputType type);
		static void Release();

		inline static bool IsKeyDown(EKey key)	{ return s_pController->IsKeyDown(key); }
		inline static bool IsKeyUp(EKey key)	{ return s_pController->IsKeyUp(key); }
		inline static bool IsMouseButtonDown(EMouseButton button)	{ return s_pController->IsMouseButtonDown(button); }
		inline static bool IsMouseButtonUp(EMouseButton button)		{ return s_pController->IsMouseButtonUp(button); }
		
		inline static void SetMousePosition(uint32 x, uint32 y)		{ s_pController->SetMousePosition(x, y); }
		inline static void GetMousePosition(uint32& x, uint32& y)	{ s_pController->GetMousePosition(x, y); }

		inline static void GetMouseState(MouseState& mouseState)			{ s_pController->GetMouseState(mouseState); }
		inline static void GetGamepadState(GamepadState& gamepadState)		{ s_pController->GetGamepadState(gamepadState); }
		inline static void GetKeyboardState(KeyboardState& keyboardState)	{ s_pController->GetKeyboardState(keyboardState); }
	private:
		static IInputController* s_pController;
	};
}
