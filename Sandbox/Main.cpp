#include "SandBox.h"
#include "Events/WindowEvent.h"
#include "System/Log.h"

int main()
{
	using namespace Lambda;

	DBG_MEMLEAK_CHECK();

	EngineParams params = {};
	params.pTitle = "Lambda Engine - Sandbox";
#if defined(LAMBDA_PLAT_WINDOWS)
	params.GraphicsDeviceApi    = GRAPHICS_API_VULKAN;
#else
    params.GraphicsDeviceApi    = GRAPHICS_API_VULKAN;
#endif
    params.Fullscreen   = false;
    params.VerticalSync = false;
    params.SampleCount  = 8;
    params.WindowWidth  = 1920;
    params.WindowHeight = 1080;

	Application* pApplication = DBG_NEW Application(params);
	pApplication->PushLayer(DBG_NEW SandBoxLayer());

	int32 result = pApplication->Run();
	delete pApplication;

	return result;
}
