#include "LambdaPch.h"

#include "Core/Log/EngineLog.h"

namespace Lambda
{
	ILog* CEngineLog::s_pLog = nullptr;

	void CEngineLog::Init()
	{
		s_pLog = DBG_NEW CLog();
	}

	void CEngineLog::Release()
	{
		SafeDelete(s_pLog);
	}
}
