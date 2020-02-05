#include "LambdaPch.h"

#include "Core/Log/Log.h"
#include "Core/Log/EngineLog.h"

namespace Lambda
{
	ILog* EngineLog::s_pLog = nullptr;
    ILog* EngineLog::s_pCoreLog = nullptr;

	void EngineLog::Init()
	{
		s_pLog      = DBG_NEW CLog();
        s_pCoreLog  = DBG_NEW CLog();
	}

	void EngineLog::Release()
	{
		SafeDelete(s_pLog);
        SafeDelete(s_pCoreLog);
	}
}
