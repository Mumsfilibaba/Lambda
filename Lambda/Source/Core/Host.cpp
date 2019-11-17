#include "LambdaPch.h"
#include "Core/Host.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "../Platform/Windows/WindowsHost.h"
#endif

namespace Lambda
{
	Host* Host::Create()
	{
#if defined(LAMBDA_PLAT_WINDOWS)
		return DBG_NEW WindowsHost();
#endif
	}
}