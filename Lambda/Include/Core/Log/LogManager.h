#pragma once
#include "LambdaCore.h"
#include "Log.h"

#include "Core/Singleton.h"

namespace Lambda
{
	class CLogManager final
	{
	public:
		LAMBDA_STATIC_CLASS(CLogManager);

		static void Init();
		static void Release();
		static ILog* GetLog() { return s_pLog; }
	private:
		CLogManager();
		~CLogManager() = default;
	private:
		static ILog* s_pLog;
	};
}