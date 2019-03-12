#include <LambdaPch.h>
#include "WindowsInput.h"

#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	Input* Input::Create()
	{
		return new WindowsInput();
	}

	WindowsInput::WindowsInput()
	{
	}
	WindowsInput::~WindowsInput()
	{
	}
	Key WindowsInput::WindowsKeyToLambda(uint32 keycode)
	{
		return Key();
	}
	uint32 WindowsInput::LambdaKeyToWindows(Key keycode)
	{
		return uint32();
	}
	bool WindowsInput::InternalIsKeyUp(Key keycode) const
	{
		return false;
	}
	bool WindowsInput::InternalIsKeyDown(Key keycode) const
	{
		return false;
	}
	void WindowsInput::InternalSetMousePosition(uint32 x, uint32 y) const
	{
	}
	void WindowsInput::InternalGetMousePosition(uint32 & x, uint32 & y) const
	{
	}
	void WindowsInput::InternalSetMousePosition(IWindow * pRelativeTo, uint32 x, uint32 y) const
	{
	}
	void WindowsInput::InternalGetMousePosition(IWindow * pRelativeTo, uint32 & x, uint32 & y) const
	{
	}
}
#endif