#include "LambdaPch.h"

#include "Core/Log/Log.h"
#include "Core/Log/EngineLog.h"

namespace Lambda
{
	ILog* Log::s_pLog = nullptr;
    ILog* Log::s_pCoreLog = nullptr;

	void Log::Init()
	{
		s_pLog      = DBG_NEW CLog();
        s_pCoreLog  = DBG_NEW CLog();
	}

	void Log::Release()
	{
		SafeDelete(s_pLog);
        SafeDelete(s_pCoreLog);
	}
}
