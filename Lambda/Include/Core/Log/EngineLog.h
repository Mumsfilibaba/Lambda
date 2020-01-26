#pragma once
#include "LambdaCore.h"
#include "Log.h"

namespace Lambda
{
	class LAMBDA_API CEngineLog final
	{
	public:
		LAMBDA_STATIC_CLASS(CEngineLog);

		static void Init();
		static void Release();
		static ILog* GetLog() { return s_pLog; }
	private:
		static ILog* s_pLog;
	};
}
