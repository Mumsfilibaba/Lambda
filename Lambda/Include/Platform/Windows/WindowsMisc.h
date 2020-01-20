#pragma once
#include "LambdaCore.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#ifdef MessageBox
		#undef MessageBox
	#endif

namespace Lambda
{
	namespace Windows
	{
		namespace Misc
		{
			LAMBDA_API void MessageBox(const char* pCaption, const char* pText, uint32 type);
			LAMBDA_API void DebuggerOutput(const char* pMessage);
		}
	}

	namespace Platform = Windows;
}
#endif