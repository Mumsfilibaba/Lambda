#pragma once
#include "LambdaCore.h"

#include "EMouse.h"
#include "EKeyboard.h"

namespace Lambda
{
	class CMouseState;
	class CGamepadState;
	class CKeyboardState;

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
		friend class CInput;
	public:
		LAMBDA_INTERFACE(IInputController);

		virtual bool IsKeyUp(EKey key) const = 0; 
		virtual bool IsKeyDown(EKey key) const = 0;

		virtual bool IsMouseButtonUp(EMouseButton button) const = 0;
		virtual bool IsMouseButtonDown(EMouseButton button) const = 0;

		virtual void SetMousePosition(uint32 x, uint32 y) = 0;
		virtual void GetMousePosition(uint32& outX, uint32& outY) = 0;

		virtual void GetMouseState(CMouseState& outMouseState) const = 0;
		virtual void GetGamepadState(CGamepadState& outGamepadState) const = 0;
		virtual void GetKeyboardState(CKeyboardState& outKeyboardState) const = 0;

		virtual void Release() = 0;
	private:
		virtual bool Init() = 0;
	};

	//------
	//CInput
	//------
	class LAMBDA_API CInput
	{
	public:
		LAMBDA_STATIC_CLASS(CInput);

		static void Initialize(EInputType type);
		static void Release();

		/*///////////////////////////*/
		static bool IsKeyDown(EKey key)
		{
			return s_pController->IsKeyDown(key);
		}

		/*/////////////////////////*/
		static bool IsKeyUp(EKey key)
		{
			return s_pController->IsKeyUp(key);
		}

		/*//////////////////////////////////////////////*/
		static bool IsMouseButtonDown(EMouseButton button)
		{
			return s_pController->IsMouseButtonDown(button);
		}

		/*////////////////////////////////////////////*/
		static bool IsMouseButtonUp(EMouseButton button)
		{
			return s_pController->IsMouseButtonUp(button);
		}

		/*////////////////////////////////////////////*/
		static void SetMousePosition(uint32 x, uint32 y)
		{
			s_pController->SetMousePosition(x, y);
		}

		/*////////////////////////////////////////////////////*/
		static void GetMousePosition(uint32& outX, uint32& outY)
		{
			s_pController->GetMousePosition(outX, outY);
		}

		/*/////////////////////////////////////////////////*/
		static void GetMouseState(CMouseState& outMouseState)
		{
			s_pController->GetMouseState(outMouseState);
		}

		/*///////////////////////////////////////////////////////*/
		static void GetGamepadState(CGamepadState& outGamepadState)
		{
			s_pController->GetGamepadState(outGamepadState);
		}

		/*//////////////////////////////////////////////////////////*/
		static void GetKeyboardState(CKeyboardState& outKeyboardState)
		{
			s_pController->GetKeyboardState(outKeyboardState);
		}
	private:
		static IInputController* s_pController;
	};
}
