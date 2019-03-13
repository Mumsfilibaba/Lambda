#include <System/Application.h>
#include <System/Log.h>
#include <crtdbg.h>
#include <string>

bool OnEvent(const Lambda::Event& event)
{
	using namespace Lambda;

	if (event.Type == EVENT_TYPE_KEYDOWN)
	{
		LOG_SYSTEM_PRINT("Key down. Repeat: %d\n", event.KeyEvent.KeyCode);
		return true;
	}
	else if (event.Type == EVENT_TYPE_TEXT)
	{
		LOG_SYSTEM_PRINT("Text event. Repeat: %c\n", (char)event.TextEvent.Character);
		return true;
	}
	else if (event.Type == EVENT_TYPE_MOUSE_MOVED)
	{
		LOG_SYSTEM_PRINT("Mouse moved. x: %d, y: %d\n", event.MouseMoveEvent.PosX, event.MouseMoveEvent.PosY);
		return true;
	}

	return false;
}

int main(int argc, const char* argv[])
{
	using namespace Lambda;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Application* pApp = new Application();

	EventLayer uiLayer = { OnEvent, "UILayer" };
	EventDispatcher::PushEventLayer(uiLayer);
	
	int32 result = pApp->Run();
	delete pApp;

	return result;
}