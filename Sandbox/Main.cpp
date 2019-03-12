#include <System/Application.h>
#include <crtdbg.h>

bool OnEvent(const Lambda::Event& event)
{
	using namespace Lambda;

	if (event.Type == EVENT_TYPE_KEY)
	{
		std::cout << "Key event. Repeat: " << event.KeyEvent.RepeatCount << std::endl;
		sizeof(event.KeyEvent);
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