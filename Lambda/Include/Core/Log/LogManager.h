#pragma once
#include "LambdaCore.h"

#include "Log.h"

#include "Core/Engine/Singleton.h"

#include <string>
#include <unordered_map>

//Default log
#define LOG(verbosity, format, ...) Lambda::LogManager::Get().GetDefaultLog()->Write(verbosity, format, __VA_ARGS__) 
#define LOG_INFO(format, ...)		LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_INFO, format, __VA_ARGS__)
#define LOG_MESSAGE(format, ...)	LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_MESSAGE, format, __VA_ARGS__)
#define LOG_WARNING(format, ...)	LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_WARNING, format, __VA_ARGS__)
#define LOG_ERROR(format, ...)		LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_ERROR, format, __VA_ARGS__)

//Named log
#define N_LOG(log, verbosity, format, ...) Lambda::LogManager::Get().GetLog(log)->Write(verbosity, format, __VA_ARGS__) 
#define N_LOG_INFO(log, format, ...)	   N_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_INFO, format, __VA_ARGS__)
#define N_LOG_MESSAGE(log, format, ...)	   N_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_MESSAGE, format, __VA_ARGS__)
#define N_LOG_WARNING(log, format, ...)	   N_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_WARNING, format, __VA_ARGS__)
#define N_LOG_ERROR(log, format, ...)	   N_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_ERROR, format, __VA_ARGS__)

//Debug log
#if defined(LAMBDA_DEBUG)
	#define D_LOG(verbosity, format, ...)		LOG(verbosity, format, __VA_ARGS__)
	#define DN_LOG(log, verbosity, format, ...) N_LOG(log, verbosity, format, __VA_ARGS__)
#else
	#define D_LOG(log, verbosity, format, ...)
	#define DN_LOG(log, verbosity, format, ...)
#endif

//Default logs
#define D_LOG_INFO(format, ...)		D_LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_INFO, format, __VA_ARGS__)
#define D_LOG_MESSAGE(format, ...)	D_LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_MESSAGE, format, __VA_ARGS__)
#define D_LOG_WARNING(format, ...)	D_LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_WARNING, format, __VA_ARGS__)
#define D_LOG_ERROR(format, ...)	D_LOG(Lambda::ELogVerbosity::LOG_VERBOSITY_ERROR, format, __VA_ARGS__)

//Named logs
#define DN_LOG_INFO(log, format, ...)	 DN_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_INFO, format, __VA_ARGS__)
#define DN_LOG_MESSAGE(log, format, ...) DN_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_MESSAGE, format, __VA_ARGS__)
#define DN_LOG_WARNING(log, format, ...) DN_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_WARNING, format, __VA_ARGS__)
#define DN_LOG_ERROR(log, format, ...)	 DN_LOG(log, Lambda::ELogVerbosity::LOG_VERBOSITY_ERROR, format, __VA_ARGS__)

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