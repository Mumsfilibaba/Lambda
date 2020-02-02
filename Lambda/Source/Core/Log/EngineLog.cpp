#include "LambdaPch.h"

#include "Core/Log/Log.h"
#include "Core/Log/EngineLog.h"

namespace Lambda
{
	ILog* CEngineLog::s_pLog = nullptr;
    ILog* CEngineLog::s_pCoreLog = nullptr;

	void CEngineLog::Init()
	{
		s_pLog      = DBG_NEW CLog();
        s_pCoreLog  = DBG_NEW CLog();
	}

	void CEngineLog::Release()
	{
		SafeDelete(s_pLog);
        SafeDelete(s_pCoreLog);
	}
}
