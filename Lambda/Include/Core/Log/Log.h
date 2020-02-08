#pragma once
#include "ILog.h"

namespace Lambda
{
	class CLog final : public ILog
	{
	public:
		CLog();
		~CLog() = default;

		LAMBDA_DECL_NO_COPY(CLog);

		virtual void Enable(bool bEnable) override final;

		virtual bool IsEnabled() const override final;

		virtual void Print(ELogVerbosity verbosity, const char* pFormat, ...) override final;
		virtual void PrintV(ELogVerbosity verbosity, const char* pFormat, va_list args) override final;

	private:
		bool m_bEnabled;
	};
}
