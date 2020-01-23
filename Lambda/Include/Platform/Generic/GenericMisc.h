#pragma once
#include "LambdaCore.h"

#ifdef MessageBox
	#undef MessageBox
#endif

namespace Lambda
{
	class LAMBDA_API CGenericMisc
	{
	public:
		LAMBDA_STATIC_CLASS(CGenericMisc);

		static void MessageBox(const char*, const char*, uint32) {}
		static void DebuggerOutput() {}
	};
}
