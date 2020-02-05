#pragma once
#include "ILog.h"

namespace Lambda
{
	class LAMBDA_API EngineLog final
	{
	public:
		LAMBDA_DECL_STATIC_CLASS(EngineLog);

		static void Init();
		static void Release();

        _forceinline static ILog* GetCoreLog()   
		{ 
			return s_pCoreLog; 
		}

		_forceinline static ILog* GetLog()       
		{ 
			return s_pLog; 
		}

	private:
		static ILog* s_pLog;
        static ILog* s_pCoreLog;
	};
}

#define LOG(verbosity, ...) Lambda::EngineLog::GetLog()->Print(verbosity, __VA_ARGS__)
#define LOG_INFO(...)       LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_INFO, __VA_ARGS__)
#define LOG_MESSAGE(...)    LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_MESSAGE, __VA_ARGS__)
#define LOG_WARNING(...)    LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_WARNING, __VA_ARGS__)
#define LOG_ERROR(...)      LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_ERROR, __VA_ARGS__)

#define LOG_CORE(verbosity, ...) Lambda::EngineLog::GetCoreLog()->Print(verbosity, __VA_ARGS__)
#define LOG_CORE_INFO(...)       LOG_CORE(Lambda::ELogVerbosity::LOG_VERBOSITY_INFO, __VA_ARGS__)
#define LOG_CORE_MESSAGE(...)    LOG_CORE(Lambda::ELogVerbosity::LOG_VERBOSITY_MESSAGE, __VA_ARGS__)
#define LOG_CORE_WARNING(...)    LOG_CORE(Lambda::ELogVerbosity::LOG_VERBOSITY_WARNING, __VA_ARGS__)
#define LOG_CORE_ERROR(...)      LOG_CORE(Lambda::ELogVerbosity::LOG_VERBOSITY_ERROR, __VA_ARGS__)

//Debug log
#if defined(LAMBDA_DEBUG)
    #define D_LOG(verbosity, ...)           LOG(verbosity, __VA_ARGS__)
    #define D_LOG_CORE(verbosity, ...)      LOG_CORE(verbosity, __VA_ARGS__)
#else
    #define D_LOG(verbosity, ...)
    #define D_LOG_CORE(verbosity, ...)
#endif

//Default logs
#define D_LOG_INFO(...)         D_LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_INFO, __VA_ARGS__)
#define D_LOG_MESSAGE(...)      D_LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_MESSAGE, __VA_ARGS__)
#define D_LOG_WARNING(...)      D_LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_WARNING, __VA_ARGS__)
#define D_LOG_ERROR(...)        D_LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_ERROR, __VA_ARGS__)

#define D_LOG_CORE_INFO(...)         D_LOG_CORE(Lambda::ELogVerbosity::LOG_VERBOSITY_INFO, __VA_ARGS__)
#define D_LOG_CORE_MESSAGE(...)      D_LOG_CORE(Lambda::ELogVerbosity::LOG_VERBOSITY_MESSAGE, __VA_ARGS__)
#define D_LOG_CORE_WARNING(...)      D_LOG_CORE(Lambda::ELogVerbosity::LOG_VERBOSITY_WARNING, __VA_ARGS__)
#define D_LOG_CORE_ERROR(...)        D_LOG_CORE(Lambda::ELogVerbosity::LOG_VERBOSITY_ERROR, __VA_ARGS__)
