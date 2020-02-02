#pragma once
#include "LambdaCore.h"

#include <stdarg.h>

namespace Lambda
{
	enum class ELogVerbosity
	{
		LOG_VERBOSITY_UNKNOWN = 0,
		LOG_VERBOSITY_INFO = 1,
		LOG_VERBOSITY_WARNING = 2,
		LOG_VERBOSITY_ERROR = 3,
	};

	class LAMBDA_API ILog
	{
	public:
		LAMBDA_DECL_INTERFACE(ILog);

		virtual void Enable(bool bEnable) = 0;

		virtual bool IsEnabled() const = 0;

		virtual void Print(ELogVerbosity verbosity, const char* pFormat, ...) = 0;
		virtual void PrintV(ELogVerbosity verbosity, const char* pFormat, va_list args) = 0;
	};
}