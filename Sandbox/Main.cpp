#include "SandBox.h"
#include <System/Log.h>
#include <System/Clock.h>
#include <Math/Math.h>
#include <Math/Float3.h>
#include <string>
#include <crtdbg.h>

int main()
{
	using namespace Lambda;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	EngineParams params = {};
	params.pTitle = "Lambda Engine - Sandbox [DX12]";

	Application* pApp = DBG_NEW SandBox(params);
	int32 result = pApp->Run();
	delete pApp;

	return result;
}