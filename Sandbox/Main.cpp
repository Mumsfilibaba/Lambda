#include "SandBox.h"
#include "Events/WindowEvent.h"
#include "Core/LogManager.h"
#include "Core/LEngine.h"

int main(int argc, const char* argv[])
{
	using namespace Lambda;

	Layer* pLayer = DBG_NEW SandBoxLayer();

	LEngineParams params = {};
	params.ppCmdArgs	= argv;
	params.CmdArgsCount = argc;
	params.ppLayers		= &pLayer;
	params.LayerCount	= 1;

	return LambdaMain(params);
}
