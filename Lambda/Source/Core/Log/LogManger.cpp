#include "LambdaCore.h"

#include "Core/Log/LogManager.h"

namespace Lambda
{
	ILog* CLogManager::s_pLog = nullptr;

	void CLogManager::Init()
	{
		s_pLog = DBG_NEW CLog();
	}

	void CLogManager::Release()
	{
		SafeDelete(s_pLog);
	}
}