#include "LambdaPch.h"

#include "Core/Log/Log.h"
#include "Core/Engine/Console.h"

namespace Lambda
{
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	static const char* VerbosityToString(ELogVerbosity verbosity)
	{
		if (verbosity == ELogVerbosity::LOG_VERBOSITY_INFO)
			return "INFO   ";
		else if (verbosity == ELogVerbosity::LOG_VERBOSITY_MESSAGE)
			return "MESSAGE";
		else if (verbosity == ELogVerbosity::LOG_VERBOSITY_WARNING)
			return "WARNING";
		else if (verbosity == ELogVerbosity::LOG_VERBOSITY_ERROR)
			return "ERROR  ";
		else
			return "";
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	static const char* LogModeToString(ELogMode mode)
	{
		if (mode == ELogMode::LOG_MODE_APPEND)
			return "APPEND";
		else if (mode == ELogMode::LOG_MODE_TRUNCATE)
			return "TRUNCATE";
		else
			return "";
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	static const char* GetOpenMode(ELogMode mode)
	{
		if (mode == ELogMode::LOG_MODE_APPEND)
			return "a";
		else if (mode == ELogMode::LOG_MODE_TRUNCATE)
			return "w";
		else
			return "";
	}

	//----
	//CLog
	//----

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CLog::CLog(const char* pName, ELogMode mode, ELogVerbosity filter, bool bWriteConsole, bool bWriteFile)
		: ILog(),
		m_pLogFile(nullptr),
		m_Mode(mode),
		m_Filter(filter),
		m_Name(),
		m_Filename(),
		m_bConsoleOutput(bWriteConsole),
		m_bFileOutput(bWriteFile)
	{
		LAMBDA_ASSERT_PRINT(pName, "Log does not have a name");

		m_Name = std::string(pName);
		//We call our logfiles .LLog (Lambda Log)
		m_Filename = m_Name + ".LLog";

		if (m_bConsoleOutput)
		{
			Console::SetTextColor(EConsoleColor::CONSOLE_COLOR_GREEN);
			Console::PrintLine("Created Log '%s' Mode=%s, WriteConsole=%s, WriteFile=%s, Filter=%s", pName, LogModeToString(mode), bWriteConsole ? "true" : "false", bWriteFile ? "true" : "false", VerbosityToString(filter));
			Console::Reset();
		}

		if (m_bFileOutput)
		{
			OpenFile();
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CLog::~CLog()
	{
		CloseFile();
	}
	
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CLog::Write(ELogVerbosity verbosity, const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		CLog::WriteV(verbosity, pFormat, args);
		va_end(args);
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CLog::WriteV(ELogVerbosity verbosity, const char* pFormat, va_list args)
	{
		//Filter out messages
		if (verbosity > m_Filter)
		{
			return;
		}

		constexpr uint32 MAX_FORMATTED_MESSAGE_LENGTH = 512;
		static char formattedMessage[MAX_FORMATTED_MESSAGE_LENGTH];
		uint32 written = vsnprintf(formattedMessage, MAX_FORMATTED_MESSAGE_LENGTH, pFormat, args);

		static char logMessage[MAX_FORMATTED_MESSAGE_LENGTH];
		written = snprintf(logMessage, MAX_FORMATTED_MESSAGE_LENGTH, "[%s] %s : %s", m_Name.c_str(), VerbosityToString(verbosity), formattedMessage);

		//Output to console
		if (m_bConsoleOutput)
		{
			if (verbosity == ELogVerbosity::LOG_VERBOSITY_MESSAGE)
				Console::SetTextColor(EConsoleColor::CONSOLE_COLOR_WHITE);
			else if (verbosity == ELogVerbosity::LOG_VERBOSITY_INFO)
				Console::SetTextColor(EConsoleColor::CONSOLE_COLOR_GREEN);
			else if (verbosity == ELogVerbosity::LOG_VERBOSITY_WARNING)
				Console::SetTextColor(EConsoleColor::CONSOLE_COLOR_YELLOW);
			else if (verbosity == ELogVerbosity::LOG_VERBOSITY_ERROR)
				Console::SetTextColor(EConsoleColor::CONSOLE_COLOR_RED);

			Console::PrintLine(logMessage);
			Console::Reset();
		}

		//Output to file
		if (m_bFileOutput && m_pLogFile)
		{
			if (fwrite(logMessage, sizeof(char), written, m_pLogFile) != written)
			{
				Console::SetTextColor(EConsoleColor::CONSOLE_COLOR_RED);
				Console::PrintLine("Failed to write to file '%s'", m_Filename.c_str());
				Console::Reset();
			}
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CLog::SetVerbosityFilter(ELogVerbosity filter)
	{
		m_Filter= filter;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CLog::SetConsoleOutputEnabled(bool bConsoleOutput)
	{
		m_bConsoleOutput = bConsoleOutput;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CLog::SetFileOutputEnabled(bool bFileOutput)
	{
		if (!bFileOutput && m_pLogFile)
			CloseFile();
		else if (bFileOutput && m_pLogFile == nullptr)
			OpenFile();

		m_bFileOutput = bFileOutput;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool CLog::OpenFile()
	{
		//If file already is opened with this mode, we return true
		if (m_pLogFile)
		{
			return true;
		}

		//Open file
		FILE* pFile = fopen(m_Filename.c_str(), GetOpenMode(m_Mode));
		if (pFile)
		{
			if (m_bConsoleOutput)
			{
				Console::SetTextColor(EConsoleColor::CONSOLE_COLOR_GREEN);
				Console::PrintLine("Opened file '%s'", m_Filename.c_str());
				Console::Reset();
			}

			m_pLogFile = pFile;
			return true;
		}

		return false;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool CLog::CloseFile()
	{
		//If the file is not open we return false
		if (!m_pLogFile)
		{
			return false;
		}

		//Flush and close
		if (fclose(m_pLogFile) == 0)
		{
			if (m_bConsoleOutput)
			{
				Console::SetTextColor(EConsoleColor::CONSOLE_COLOR_GREEN);
				Console::PrintLine("Closed file '%s'", m_Filename.c_str());
				Console::Reset();
			}

			return true;
		}

		return false;
	}
}
