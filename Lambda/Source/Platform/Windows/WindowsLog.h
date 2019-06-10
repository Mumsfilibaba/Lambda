#pragma once
#include "System/Log.h"
#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	class WindowsLog final : public ILog
	{
	public:
		LAMBDA_NO_COPY(WindowsLog);

		WindowsLog();
		~WindowsLog();

		virtual void Print(LogSeverity severity, const char* pMessage, ...) override;
	};
}
#endif