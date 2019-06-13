#include "LambdaPch.h"
#include "System/Input.h"

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
	
    
	void Input::SetMousePosition(float x, float y)
	{
		s_Instance->InternalSetMousePosition(x, y);
	}
	
    
	void Input::GetMousePosition(float& x, float& y)
	{
		s_Instance->InternalGetMousePosition(x, y);
	}
	
    
	void Input::SetMousePosition(IWindow* pRelativeTo, float x, float y)
	{
		s_Instance->InternalSetMousePosition(pRelativeTo, x, y);
	}
    

	void Input::GetMousePosition(IWindow* pRelativeTo, float& x, float& y)
	{
		s_Instance->InternalGetMousePosition(pRelativeTo, x, y);
	}
}
