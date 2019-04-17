#pragma once
#include "Defines.h"
#include <memory>

#if defined(LAMBDA_DEBUG)
#define LOG_DEBUG_INFO(...) Log::GetDebugLog().Print(LOG_SEVERITY_INFO, __VA_ARGS__)
#define LOG_DEBUG_WARNING(...) Log::GetDebugLog().Print(LOG_SEVERITY_WARNING, __VA_ARGS__)
#define LOG_DEBUG_ERROR(...) Log::GetDebugLog().Print(LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK()

#define LOG_SYSTEM_INFO(...) Log::GetSystemLog().Print(LOG_SEVERITY_INFO, __VA_ARGS__)
#define LOG_SYSTEM_WARNING(...) Log::GetSystemLog().Print(LOG_SEVERITY_WARNING, __VA_ARGS__)
#define LOG_SYSTEM_ERROR(...) Log::GetSystemLog().Print(LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK()
#else
#define LOG_DEBUG_INFO(...)
#define LOG_DEBUG_WARNING(...)
#define LOG_DEBUG_ERROR(...)

#define LOG_SYSTEM_INFO(...)
#define LOG_SYSTEM_WARNING(...)
#define LOG_SYSTEM_ERROR(...)
#endif

#if !defined(LAMBDA_NO_LOGS)
#define LOG_DEBUG_PRINT(...) Log::GetDebugLog().Print(LOG_SEVERITY_INFO, __VA_ARGS__)
#define LOG_SYSTEM_PRINT(...) Log::GetSystemLog().Print(LOG_SEVERITY_INFO, __VA_ARGS__)
#endif

namespace Lambda
{
	enum LogSeverity : uint32
	{
		LOG_SEVERITY_UNKNOWN,
		LOG_SEVERITY_INFO,
		LOG_SEVERITY_WARNING,
		LOG_SEVERITY_ERROR,
	};

	class LAMBDA_API ILog
	{
	public:
		LAMBDA_INTERFACE(ILog);

		ILog() = default;
		~ILog() = default;

		virtual void Print(LogSeverity severity, const char* pFormat, ...) = 0;

	public:
		static ILog* Create();
	};


	class LAMBDA_API Log
	{
	public:
		LAMBDA_STATIC_CLASS(Log);

		static ILog& GetDebugLog();
		static ILog& GetSystemLog();

	private:
		static std::unique_ptr<ILog> s_DebugLog;
		static std::unique_ptr<ILog> s_SystemLog;
	};

	inline ILog& Log::GetDebugLog()
	{
		return *s_DebugLog;
	}

	inline ILog& Log::GetSystemLog()
	{
		return *s_SystemLog;
	}
}