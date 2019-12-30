#include "LambdaPch.h"

#include "Core/Engine/Console.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/CWindowsConsole.h"
#endif

namespace Lambda
{
	//--------
	//IConsole
	//--------

	////////////////////////////
	IConsole* IConsole::Create()
	{
#if defined(LAMBDA_PLAT_WINDOWS)
		return DBG_NEW CWindowsConsole();
#endif
	}
}