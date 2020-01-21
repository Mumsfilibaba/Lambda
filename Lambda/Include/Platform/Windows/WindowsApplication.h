#pragma once
#include "LambdaCore.h"

#include "Platform/Generic/GenericApplication.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#ifndef WIN32_LEAN_AND_MEAN 
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>

namespace Lambda
{
	class LAMBDA_API CWindowsApplication : public CGenericApplication
	{
	public:
		LAMBDA_STATIC_CLASS(CWindowsApplication);

		static void Init(HINSTANCE hInstance);
		static void PollEvents();

		static HINSTANCE GetApplicationInstance() { return s_hInstance; }
	private:
		static HINSTANCE s_hInstance;
	};

	typedef CWindowsApplication PlatformApplication;
}
#endif