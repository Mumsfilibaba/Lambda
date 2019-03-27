#include <System/Application.h>
#include <System/Log.h>
#include <System/Clock.h>
#include <Math/Math.h>
#include <Math/Float3.h>
#include <string>
#include <crtdbg.h>

bool OnEvent(const Lambda::Event& event)
{
	using namespace Lambda;

	if (event.Type == EVENT_TYPE_KEYDOWN)
	{
		//LOG_SYSTEM_PRINT("Key down. Repeat: %d\n", event.KeyEvent.KeyCode);
		return true;
	}
	else if (event.Type == EVENT_TYPE_TEXT)
	{
		//LOG_SYSTEM_PRINT("Text event. Repeat: %c\n", (char)event.TextEvent.Character);
		return true;
	}
	else if (event.Type == EVENT_TYPE_MOUSE_MOVED)
	{
		//LOG_SYSTEM_PRINT("Mouse moved. x: %d, y: %d\n", event.MouseMoveEvent.PosX, event.MouseMoveEvent.PosY);
		return true;
	}
	else if (event.Type == EVENT_TYPE_JOYSTICK_CHANGED)
	{
		//LOG_SYSTEM_PRINT("Joystick Changed:\nID: %d\nLeftTrigger: %d, RightTrigger: %d\nLeft Thumb x: %d, y: %d\nRight Thumb x: %d, y: %d\n",
		//	event.JoystickChanged.ControllerID, 
		//	event.JoystickChanged.LeftTrigger, event.JoystickChanged.RightTrigger,
		//	event.JoystickChanged.LeftX, event.JoystickChanged.LeftY, event.JoystickChanged.RightX, event.JoystickChanged.RightY);
		return true;
	}

	return false;
}

int main()
{
	using namespace Lambda;
	using namespace Math;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Application* pApp = DBG_NEW Application();

	EventLayer uiLayer = { OnEvent, "UILayer" };
	EventDispatcher::PushEventLayer(uiLayer);

	int32 result = pApp->Run();
	delete pApp;

	return result;
}