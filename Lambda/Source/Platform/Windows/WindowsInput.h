#pragma once
#include <System/Input.h>

#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	class WindowsInput final : public Input
	{
	public:
		LAMBDA_NO_COPY(WindowsInput);

		WindowsInput();
		~WindowsInput();

		Key WindowsKeyToLambda(uint32 keycode) const;
		uint32 LambdaKeyToWindows(Key keycode) const;

		virtual bool InternalIsKeyUp(Key keycode) const override final;
		virtual bool InternalIsKeyDown(Key keycode) const override final;

		virtual void InternalSetMousePosition(uint32 x, uint32 y) const override final;
		virtual void InternalGetMousePosition(uint32& x, uint32& y) const override final;
		virtual void InternalSetMousePosition(IWindow* pRelativeTo, uint32 x, uint32 y) const override final;
		virtual void InternalGetMousePosition(IWindow* pRelativeTo, uint32& x, uint32& y) const override final;

	private:
		Key m_WindowsToLambdaKeyTable[255];
		uint32 m_LambdaToWindowsKeyTable[129];

	public:
		static Key ConvertWindowsKey(uint32 keycode);
		static uint32 ConvertLambdaKey(Key keycode);
	};

	inline Key WindowsInput::ConvertWindowsKey(uint32 keycode)
	{
		return ((WindowsInput&)Input::GetInstance()).WindowsKeyToLambda(keycode);
	}

	inline uint32 WindowsInput::ConvertLambdaKey(Key keycode)
	{
		return ((WindowsInput&)Input::GetInstance()).LambdaKeyToWindows(keycode);
	}
}
#endif