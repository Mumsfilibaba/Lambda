#include "LambdaPch.h"

#include "Core/Log/LogManager.h"

namespace Lambda
{
	//----------
	//LogManager
	//----------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	LogManager::LogManager()
		: Singleton<LogManager>(),
		m_Logs()
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	LogManager::~LogManager()
	{
		//Delete all logs
		for (auto& log : m_Logs)
			SafeDelete(log.second);

		//Erase table
		m_Logs.clear();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	ILog* LogManager::CreateLog(const char* pName, ELogMode mode, ELogVerbosity verbosity, bool bWriteConsole, bool bWriteFile)
	{
		if (pName)
		{
			//Create log
			Log* pLog = DBG_NEW Log(pName, mode, verbosity, bWriteConsole, bWriteFile);
			
			//Insert log in table
			std::string name = std::string(pName);
			m_Logs.insert({ name, pLog });

			return pLog;
		}

		return nullptr;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void LogManager::CreateDefaultLog(ELogMode mode, ELogVerbosity verbosity, bool bWriteConsole, bool bWriteFile)
	{
		std::string name = "Engine";
		if (m_Logs.count(name) != 0)
		{
			ReleaseLog(name.c_str());
		}

		ILog* pLog = CreateLog(name.c_str(), mode, verbosity, bWriteConsole, bWriteFile);
		m_pDefaultLog = pLog;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void LogManager::ReleaseLog(const char* pName)
	{
		if (!pName)
			return;

		auto log = m_Logs.find(std::string(pName));
		if (log != m_Logs.end())
		{
			m_Logs.erase(log);
			SafeDelete(log->second);
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void LogManager::ReleaseLog(ILog* pLog)
	{
		if (!pLog)
			return;

		const char* pName = pLog->GetName();
		ReleaseLog(pName);
	}
	
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	ILog* LogManager::GetLog(const char* pName) const
	{
		if (pName)
		{
			//Insert log in table
			std::string name = std::string(pName);
			if (m_Logs.count(name) != 0)
			{
				auto log = m_Logs.find(name);
				return log->second;
			}
		}

		return nullptr;
	}
	
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	LogManager* LogManager::Create()
	{
		return DBG_NEW LogManager();
	}
}