#pragma once
#include "LambdaCore.h"

#include "Log.h"

#include "Core/Engine/Singleton.h"

#include <string>
#include <unordered_map>

//Default log
#define LOG(verbosity, ...) Lambda::LogManager::Get().GetDefaultLog()->Write(verbosity, __VA_ARGS__)
#define LOG_INFO(...)		LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_INFO, __VA_ARGS__)
#define LOG_MESSAGE(...)	LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_MESSAGE, __VA_ARGS__)
#define LOG_WARNING(...)	LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_WARNING, __VA_ARGS__)
#define LOG_ERROR(...)		LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_ERROR, __VA_ARGS__)

//Named log
#define N_LOG(log, verbosity, ...) Lambda::LogManager::Get().GetLog(log)->Write(verbosity, __VA_ARGS__)
#define N_LOG_INFO(log, ...)	   N_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_INFO, __VA_ARGS__)
#define N_LOG_MESSAGE(log, ...)	   N_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_MESSAGE, __VA_ARGS__)
#define N_LOG_WARNING(log, ...)	   N_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_WARNING, __VA_ARGS__)
#define N_LOG_ERROR(log, ...)	   N_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_ERROR, __VA_ARGS__)

//Debug log
#if defined(LAMBDA_DEBUG)
	#define D_LOG(verbosity, ...)		LOG(verbosity, __VA_ARGS__)
	#define DN_LOG(log, verbosity, ...) N_LOG(log, verbosity, __VA_ARGS__)
#else
	#define D_LOG(verbosity, ...)
	#define DN_LOG(log, verbosity, ...)
#endif

//Default logs
#define D_LOG_INFO(...)		D_LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_INFO, __VA_ARGS__)
#define D_LOG_MESSAGE(...)	D_LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_MESSAGE, __VA_ARGS__)
#define D_LOG_WARNING(...)	D_LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_WARNING, __VA_ARGS__)
#define D_LOG_ERROR(...)	D_LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_ERROR, __VA_ARGS__)

//Named logs
#define DN_LOG_INFO(log, ...)	 DN_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_INFO, __VA_ARGS__)
#define DN_LOG_MESSAGE(log, ...) DN_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_MESSAGE, __VA_ARGS__)
#define DN_LOG_WARNING(log, ...) DN_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_WARNING, __VA_ARGS__)
#define DN_LOG_ERROR(log, ...)	 DN_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_ERROR, __VA_ARGS__)

namespace Lambda
{
	//----------
	//LogManager
	//----------

	class LAMBDA_API LogManager final : public Singleton<LogManager>
	{
	private:
		LogManager();
	public:
		~LogManager();

		ILog* CreateLog(const char* pName, ELogMode mode, ELogVerbosity verbosity, bool bWriteConsole, bool bWriteFile);
		void CreateDefaultLog(ELogMode mode, ELogVerbosity verbosity, bool bWriteConsole, bool bWriteFile);
		
		void ReleaseLog(const char* pName);
		void ReleaseLog(ILog* pLog);

		ILog* GetLog(const char* pName) const;

		ILog* GetDefaultLog() const { return m_pDefaultLog; }
		void SetDefaultLog(ILog* pLog) 
		{ 
			if (pLog) 
			{ 
				m_pDefaultLog = pLog; 
			}
		}
	private:
		ILog* m_pDefaultLog;
		std::unordered_map<std::string, ILog*> m_Logs;
	public:
		static LogManager* Create();
	};
}
