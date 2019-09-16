#include "SandBox.h"

int main()
{
	using namespace Lambda;

	DBG_MEMLEAK_CHECK();

	EngineParams params = {};
	params.pTitle               = "Lambda Engine - Sandbox";
#if defined(LAMBDA_PLAT_WINDOWS)
	params.GraphicsDeviceApi    = GRAPHICS_API_D3D12;
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
