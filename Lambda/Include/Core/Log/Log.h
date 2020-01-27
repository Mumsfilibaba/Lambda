#pragma once
#include "LambdaCore.h"

#include <stdarg.h>

namespace Lambda
{
	enum class ELogVerbosity
	{
		LOG_VERBOSITY_UNKNOWN	= 0,
		LOG_VERBOSITY_INFO		= 1,
		LOG_VERBOSITY_WARNING	= 2,
		LOG_VERBOSITY_ERROR		= 3,
	};

	class ILog
	{
	public:
		LAMBDA_INTERFACE(ILog);

		virtual void Enable(bool bEnable) = 0;

		virtual bool IsEnabled() const = 0;

		virtual void Print(ELogVerbosity verbosity, const char* pFormat, ...) = 0;
		virtual void PrintV(ELogVerbosity verbosity, const char* pFormat, va_list args) = 0;
	};

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
