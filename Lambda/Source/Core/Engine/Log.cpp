#include "LambdaPch.h"

#include "Core/Engine/Log.h"
#include "Core/Engine/Console.h"

namespace Lambda
{
	//----
	//CLog
	//----

	/*/////////////////////////////////////////////////////////////////////*/
	CLog::CLog(const char* pName, ELogMode mode, ELogVerbosity filter)
		: ILog(),
		m_pLogFile(nullptr),
		m_Mode(mode),
		m_Filter(filter),
		m_Name(),
		m_Buffer(),
		m_bConsoleOutput(true),
		m_bFileOutput(true)
	{
		if (pName)
		{
			m_Name = std::string(pName);
		}
	}

	/*/////////*/
	CLog::~CLog()
	{
		CloseFile();
	}
	
	/*/////////////////////////////////////////////////////////////////////*/
	void CLog::Log(ELogVerbosity verbosity, const char* pFormat, va_list args)
	{
		if (verbosity >= m_Filter)
		{
			if (m_bConsoleOutput)
			{
				IConsole* pConsole = CConsole::Get();
				if (pConsole)
				{
					pConsole->PrintLine(pFormat, args);
				}
			}

			if (m_bFileOutput)
			{
				if (m_pLogFile)
				{
					//LOG TO FILE
				}
			}
		}
	}

	/*/////////////////////////////////////////////////////////////////////*/
	void CLog::SetVerbosityFilter(ELogVerbosity filter)
	{
		m_Filter= filter;
	}

	/*/////////////////////////////////////////////////////////////////////*/
	void CLog::SetConsoleOutputEnabled(bool bConsoleOutput)
	{
		m_bConsoleOutput = bConsoleOutput;
	}

	/*/////////////////////////////////////////////////////////////////////*/
	void CLog::SetFileOutputEnabled(bool bFileOutput)
	{
		if (!bFileOutput && m_pLogFile)
		{
			CloseFile();
		}
		else if (bFileOutput && m_pLogFile == nullptr)
		{

		}

		m_bFileOutput = bFileOutput;
	}

	/*/////////////////////////////////////////////////////////////////////*/
	void CLog::Flush()
	{
	}

	/*/////////////////////////////////////////////////////////////////////*/
	bool CLog::OpenFile()
	{
		//If file already is opened with this mode, we return true
		if (m_pLogFile)
		{
			return true;
		}

		//Filemode
		const char* pMode = "";
		if (m_Mode == ELogMode::LOG_MODE_APPEND)
			pMode = "a";
		else if (m_Mode == ELogMode::LOG_MODE_TRUNCATE)
			pMode = "w";

		//We call our logfiles .LLog (Lambda Log)
		std::string filename = m_Name + ".LLog";

		//Open file
		FILE* pFile = fopen(filename.c_str(), pMode);
		if (pFile)
		{
			return true;
		}

		return false;
	}

	/*/////////////////////////////////////////////////////////////////////*/
	bool CLog::CloseFile()
	{
		//If the file is not open we return false
		if (!m_pLogFile)
		{
			return false;
		}

		//Flush and close
		CLog::Flush();
		if (fclose(m_pLogFile) == 0)
		{
			return true;
		}

		return false;
	}
}