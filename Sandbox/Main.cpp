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
	using namespace Math;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Application* pApp = DBG_NEW SandBox();
	int32 result = pApp->Run();
	delete pApp;

	return result;
}