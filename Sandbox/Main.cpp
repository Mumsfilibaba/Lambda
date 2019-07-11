#include "SandBox.h"

int main()
{
	using namespace Lambda;

	DBG_MEMLEAK_CHECK();

	EngineParams params;
	params.pTitle = "Lambda Engine - Sandbox [Vulkan]";

	Application* pApp = DBG_NEW SandBox(params);
	int32 result = pApp->Run();
	delete pApp;

	return result;
}
