#include "SandBox.h"
#include "Events/WindowEvent.h"
#include "System/Log.h"

class Test
{
public:
	bool OnResizeEvent(const Lambda::WindowResizeEvent& event)
	{
		LOG_DEBUG_INFO("Test::OnResizeEvent\n");
		return true;
	}
};

int main()
{
	using namespace Lambda;

	DBG_MEMLEAK_CHECK();

	EngineParams params = {};
	params.pTitle               = "Lambda Engine - Sandbox";
#if defined(LAMBDA_PLAT_WINDOWS)
	params.GraphicsDeviceApi    = GRAPHICS_API_VULKAN;
#else
	params.GraphicsDeviceApi	= GRAPHICS_API_VULKAN;
#endif
	params.SampleCount          = 1;
	params.WindowWidth          = 1920;
	params.WindowHeight         = 1080;
	params.Fullscreen           = false;

	Application* pApp = DBG_NEW SandBox(params);
	int32 result = pApp->Run();
	delete pApp;

	return result;
}
