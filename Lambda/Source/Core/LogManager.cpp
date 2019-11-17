#include "LambdaPch.h"
#include "Core/LogManager.h"

namespace Lambda
{
	//----------
	//LogManager
	//----------

	LogManager::LogManager()
		: m_DebugLog(ILog::Create()),
		m_SystemLog(ILog::Create())
	{
	}
}