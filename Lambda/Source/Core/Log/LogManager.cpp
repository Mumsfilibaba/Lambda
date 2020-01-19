#include "LambdaPch.h"

#include "Core/Log/LogManager.h"

namespace Lambda
{
	//-----------
	//CLogManager
	//-----------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CLogManager::CLogManager()
		: CSingleton<CLogManager>(),
		m_pDefaultLog(nullptr),
		m_Logs()
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CLogManager::~CLogManager()
	{
		//Delete all logs
		for (auto& log : m_Logs)
			SafeDelete(log.second);

		//Erase table
		m_Logs.clear();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	ILog* CLogManager::CreateLog(const char* pName, ELogMode mode, ELogVerbosity verbosity, bool bWriteConsole, bool bWriteFile)
	{
		if (pName)
		{
			//Create log
			CLog* pLog = DBG_NEW CLog(pName, mode, verbosity, bWriteConsole, bWriteFile);
			
			//Insert log in table
			std::string name = std::string(pName);
			m_Logs.insert({ name, pLog });

			return pLog;
		}

		return nullptr;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CLogManager::CreateDefaultLog(ELogMode mode, ELogVerbosity verbosity, bool bWriteConsole, bool bWriteFile)
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
	void CLogManager::ReleaseLog(const char* pName)
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
	void CLogManager::ReleaseLog(ILog* pLog)
	{
		if (!pLog)
			return;

		const char* pName = pLog->GetName();
		ReleaseLog(pName);
	}
	
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	ILog* CLogManager::GetLog(const char* pName) const
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
	CLogManager* CLogManager::Create()
	{
		return DBG_NEW CLogManager();
	}
}
