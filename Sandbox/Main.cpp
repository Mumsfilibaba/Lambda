#include "SandBox.h"

int main()
{
	using namespace Lambda;

	DBG_MEMLEAK_CHECK();

	EngineParams params = {};
	params.pTitle = "Lambda Engine - Sandbox [Vulkan]";
	params.GraphicsDeviceApi = GRAPHICS_API_VULKAN;
	params.SampleCount = 4;
	params.WindowWidth = 1920;
	params.WindowHeight = 1080;
	params.Fullscreen = false;

	Application* pApp = DBG_NEW SandBox(params);
	int32 result = pApp->Run();
	delete pApp;

	return result;
}
