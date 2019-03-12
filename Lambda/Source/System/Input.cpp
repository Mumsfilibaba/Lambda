#include <LambdaPch.h>
#include <System/Input.h>

namespace Lambda
{
	std::unique_ptr<Input> Input::s_Instance = std::unique_ptr<Input>(Input::Create());

	bool Input::IsKeyUp(Key keycode)
	{
		return s_Instance->InternalIsKeyUp(keycode);
	}
	
	bool Input::IsKeyDown(Key keycode)
	{
		return s_Instance->InternalIsKeyDown(keycode);
	}
	
	void Input::SetMousePosition(uint32 x, uint32 y)
	{
		s_Instance->InternalSetMousePosition(x, y);
	}
	
	void Input::GetMousePosition(uint32& x, uint32& y)
	{
		s_Instance->InternalGetMousePosition(x, y);
	}
	
	void Input::SetMousePosition(IWindow* pRelativeTo, uint32 x, uint32 y)
	{
		s_Instance->InternalSetMousePosition(pRelativeTo, x, y);
	}

	void Input::GetMousePosition(IWindow* pRelativeTo, uint32& x, uint32& y)
	{
		s_Instance->InternalGetMousePosition(pRelativeTo, x, y);
	}
}