#include "SandBox.h"
#include "System/Log.h"
#include "System/Clock.h"
#include "Math/Math.h"
#include "Math/Float3.h"
#include <string>

int main()
{
	using namespace Lambda;

	DBG_MEMLEAK_CHECK();

	EngineParams params;
	params.pTitle = "Lambda Engine - Sandbox [DX12]";

	Application* pApp = DBG_NEW SandBox(params);
	int32 result = pApp->Run();
	delete pApp;

	return result;
}