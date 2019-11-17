#pragma once
#include "LambdaCore.h"
#include "Utilities/Singleton.h"
#include <memory>

//Log
#if defined(LAMBDA_DEBUG)
	#define LOG_DEBUG_INFO(...) Lambda::LogManager::Get().GetDebugLog().Print(Lambda::LOG_SEVERITY_INFO, __VA_ARGS__)
	#define LOG_DEBUG_WARNING(...) Lambda::LogManager::Get().GetDebugLog().Print(Lambda::LOG_SEVERITY_WARNING, __VA_ARGS__)
	#define LOG_DEBUG_ERROR(...) Lambda::LogManager::Get().GetDebugLog().Print(Lambda::LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK
	
	#define LOG_SYSTEM_INFO(...) Lambda::LogManager::Get().GetSystemLog().Print(Lambda::LOG_SEVERITY_INFO, __VA_ARGS__)
	#define LOG_SYSTEM_WARNING(...) Lambda::LogManager::Get().GetSystemLog().Print(Lambda::LOG_SEVERITY_WARNING, __VA_ARGS__)
	#define LOG_SYSTEM_ERROR(...) Lambda::LogManager::Get().GetSystemLog().Print(Lambda::LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK
#else
	#define LOG_DEBUG_INFO(...)
	#define LOG_DEBUG_WARNING(...)
	#define LOG_DEBUG_ERROR(...)
	
	#define LOG_SYSTEM_INFO(...)
	#define LOG_SYSTEM_WARNING(...)
	#define LOG_SYSTEM_ERROR(...)
#endif

#if !defined(LAMBDA_NO_LOGS)
	#define LOG_DEBUG(...) Lambda::LogManager::Get().GetDebugLog().Print(__VA_ARGS__)
	#define LOG_SYSTEM(...) Lambda::LogManager::Get().GetSystemLog().Print(__VA_ARGS__)
	#define LOG_DEBUG_PRINT(...) Lambda::LogManager::Get().GetDebugLog().Print(Lambda::LOG_SEVERITY_INFO, __VA_ARGS__)
	#define LOG_SYSTEM_PRINT(...) Lambda::LogManager::Get().GetSystemLog().Print(Lambda::LOG_SEVERITY_INFO, __VA_ARGS__)
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
    //-----------
    //LogSeverity
    //-----------
    
	enum LogSeverity : uint32
	{
		LOG_SEVERITY_UNKNOWN    = 0,
		LOG_SEVERITY_INFO       = 1,
		LOG_SEVERITY_WARNING    = 2,
		LOG_SEVERITY_ERROR      = 3,
        LOG_SEVERITY_MESSAGE    = 4,
	};

    //----
    //ILog
    //----
    
	class LAMBDA_API ILog
	{
	public:
		LAMBDA_INTERFACE(ILog);

		virtual void Print(LogSeverity severity, const char* pFormat, ...) = 0;
	public:
		static ILog* Create();
	};

    //----------
    //LogManager
    //----------

	class LAMBDA_API LogManager : public Singleton<LogManager>
	{
	public:
		LogManager();
		~LogManager() = default;

		_forceinline ILog& GetDebugLog()
		{
			return *m_DebugLog;
		}


		_forceinline ILog& GetSystemLog()
		{
			return *m_SystemLog;
		}

		
		_forceinline void Release()
		{
			delete this;
		}
	private:
		std::unique_ptr<ILog> m_DebugLog;
		std::unique_ptr<ILog> m_SystemLog;
	};
}
