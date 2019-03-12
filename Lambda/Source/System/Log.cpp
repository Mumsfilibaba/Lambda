#include <LambdaPch.h>
#include <System/Log.h>

namespace Lambda
{
	std::unique_ptr<ILog> Log::s_DebugLog = std::unique_ptr<ILog>(ILog::Create());
	std::unique_ptr<ILog> Log::s_SystemLog = std::unique_ptr<ILog>(ILog::Create());
}