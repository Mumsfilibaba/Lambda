#include "LambdaPch.h"

#include "Core/Engine/Log.h"
#include "Core/Engine/Console.h"

namespace Lambda
{
	//----
	//Log
	//----

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	Log::Log(const char* pName, ELogMode mode, ELogVerbosity filter)
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

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	Log::~Log()
	{
		CloseFile();
	}
	
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Log::Write(ELogVerbosity verbosity, const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		Log::WriteV(verbosity, pFormat, args);
		va_end(args);
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Log::WriteV(ELogVerbosity verbosity, const char* pFormat, va_list args)
	{
		if (verbosity >= m_Filter)
		{
			if (m_bConsoleOutput)
			{
				Console::PrintLine(pFormat, args);
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

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Log::SetVerbosityFilter(ELogVerbosity filter)
	{
		m_Filter= filter;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Log::SetConsoleOutputEnabled(bool bConsoleOutput)
	{
		m_bConsoleOutput = bConsoleOutput;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Log::SetFileOutputEnabled(bool bFileOutput)
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

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Log::Flush()
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool Log::OpenFile()
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

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool Log::CloseFile()
	{
		//If the file is not open we return false
		if (!m_pLogFile)
		{
			return false;
		}

		//Flush and close
		Log::Flush();
		if (fclose(m_pLogFile) == 0)
		{
			return true;
		}

		return false;
	}
}
