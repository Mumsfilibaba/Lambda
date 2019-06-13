#pragma once
#include "System/Input.h"
#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	class WindowsInput final : public Input
	{
	public:
		LAMBDA_NO_COPY(WindowsInput);

		WindowsInput();
		~WindowsInput() = default;

		Key WindowsKeyToLambda(uint32 keycode) const;
		uint32 LambdaKeyToWindows(Key keycode) const;
		MouseButton WindowsButtonToLambda(uint32 keycode) const;
		uint32 LambdaButtonToWindows(MouseButton keycode) const;

		virtual bool InternalIsKeyUp(Key keycode) const override final;
		virtual bool InternalIsKeyDown(Key keycode) const override final;

		virtual void InternalSetMousePosition(float x, float y) const override final;
		virtual void InternalGetMousePosition(float& x, float& y) const override final;
		virtual void InternalSetMousePosition(IWindow* pRelativeTo, float x, float y) const override final;
		virtual void InternalGetMousePosition(IWindow* pRelativeTo, float& x, float& y) const override final;

	private:
		Key m_WindowsToLambdaKeyTable[255];
		uint32 m_LambdaToWindowsKeyTable[129];

	public:
		static Key ConvertWindowsKey(uint32 keycode);
		static uint32 ConvertLambdaKey(Key keycode);
		static MouseButton ConvertWindowsButton(uint32 keycode);
		static uint32 ConvertLambdaButton(MouseButton keycode);
	};

    
	inline Key WindowsInput::ConvertWindowsKey(uint32 keycode)
	{
		return ((WindowsInput&)Input::GetInstance()).WindowsKeyToLambda(keycode);
	}

    
	inline uint32 WindowsInput::ConvertLambdaKey(Key keycode)
	{
		return ((WindowsInput&)Input::GetInstance()).LambdaKeyToWindows(keycode);
	}

    
	inline MouseButton WindowsInput::ConvertWindowsButton(uint32 keycode)
	{
		return ((WindowsInput&)Input::GetInstance()).WindowsButtonToLambda(keycode);
	}

    
	inline uint32 WindowsInput::ConvertLambdaButton(MouseButton keycode)
	{
		return ((WindowsInput&)Input::GetInstance()).LambdaButtonToWindows(keycode);
	}
}
#endif
