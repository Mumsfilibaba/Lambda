#include "LambdaPch.h"

#include "Core/Platform.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsPlatform.h"
#elif defined(LAMBDA_PLAT_MACOS)

#endif

namespace Lambda
{
	//---------
	//CPlatform
	//---------

	void CPlatform::MessageBox(const char* pCaption, const char* pText, EMessageBoxType type)
	{
#if defined(LAMBDA_PLAT_WINDOWS)
		CWindowsPlatform::MessageBox(pCaption, pText, type);
#elif defined(LAMBDA_PLAT_MACOS)
#endif
	}


	uint64 CPlatform::QueryPerformanceCounter()
	{
#if defined(LAMBDA_PLAT_WINDOWS)
		return CWindowsPlatform::QueryPerformanceCounter();
#elif defined(LAMBDA_PLAT_MACOS)
#endif
	}


	uint64 CPlatform::QueryPerformanceFrequency()
	{
#if defined(LAMBDA_PLAT_WINDOWS)
		return CWindowsPlatform::QueryPerformanceFrequency();
#elif defined(LAMBDA_PLAT_MACOS)
#endif
	}
}