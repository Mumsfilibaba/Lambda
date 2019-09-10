#pragma once
#include "Defines.h"
#include "Types.h"
#include <memory>

//Log
#if defined(LAMBDA_DEBUG)
	#define LOG_DEBUG_INFO(...) Log::GetDebugLog().Print(LOG_SEVERITY_INFO, __VA_ARGS__)
	#define LOG_DEBUG_WARNING(...) Log::GetDebugLog().Print(LOG_SEVERITY_WARNING, __VA_ARGS__)
	#define LOG_DEBUG_ERROR(...) Log::GetDebugLog().Print(LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK
	
	#define LOG_SYSTEM_INFO(...) Log::GetSystemLog().Print(LOG_SEVERITY_INFO, __VA_ARGS__)
	#define LOG_SYSTEM_WARNING(...) Log::GetSystemLog().Print(LOG_SEVERITY_WARNING, __VA_ARGS__)
	#define LOG_SYSTEM_ERROR(...) Log::GetSystemLog().Print(LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK
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


//Asserts
#ifdef LAMBDA_DEBUG
	#define LAMBDA_ENABLE_ASSERTS
#endif

#ifdef LAMBDA_ENABLE_ASSERTS
	#define LAMBDA_ASSERT(x)			{ if(!(x)) { LOG_DEBUG_ERROR("Assertion Failed\n"); DEBUG_BREAK; } }
	#define LAMBDA_ASSERT_PRINT(x, ...) { if(!(x)) { LOG_DEBUG_ERROR(__VA_ARGS__); DEBUG_BREAK; } }
#else
	#define LAMBDA_ASSERT(x)		
	#define LAMBDA_ASSERT_PRINT(x, ...)
#endif


namespace Lambda
{
	enum LogSeverity : uint32
	{
		LOG_SEVERITY_UNKNOWN    = 0,
		LOG_SEVERITY_INFO       = 1,
		LOG_SEVERITY_WARNING    = 2,
		LOG_SEVERITY_ERROR      = 3,
        LOG_SEVERITY_MESSAGE    = 4,
	};

    
	class LAMBDA_API ILog
	{
	public:
		LAMBDA_INTERFACE(ILog);

		ILog() = default;
		virtual ~ILog() = default;

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
