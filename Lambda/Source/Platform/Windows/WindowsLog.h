#pragma once
#include "System/Log.h"
#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	//----------
	//WindowsLog
	//----------

	class WindowsLog final : public ILog
	{
	public:
		LAMBDA_NO_COPY(WindowsLog);

		WindowsLog() = default;
		~WindowsLog() = default;

		virtual void Print(LogSeverity severity, const char* pMessage, ...) override final;
	};
}
#endif
