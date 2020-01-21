#pragma once
#include "LambdaCore.h"

#include "Platform/Generic/GenericMisc.h"

#ifdef LAMBDA_PLAT_WINDOWS

namespace Lambda
{
	class LAMBDA_API CWindowsMisc : public CGenericMisc
	{
	public:
		LAMBDA_STATIC_CLASS(CWindowsMisc);

		static void MessageBox(const char* pCaption, const char* pText, uint32 type);
		static void DebuggerOutput(const char* pMessage);
	};

	typedef CWindowsMisc PlatformMisc;
}
#endif