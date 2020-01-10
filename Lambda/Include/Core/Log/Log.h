#pragma once
#include "LambdaCore.h"

#include <stdarg.h>
#include <string>

namespace Lambda
{
	//-------------
	//ELogVerbosity
	//-------------

	enum class ELogVerbosity : uint32
	{
		LOG_VERBOSITY_MESSAGE	= 1,
		LOG_VERBOSITY_INFO		= 2,
		LOG_VERBOSITY_WARNING	= 3,
		LOG_VERBOSITY_ERROR		= 4,
	};

	//--------
	//ELogMode
	//--------
	
	enum class ELogMode
	{
		LOG_MODE_APPEND		= 1,
		LOG_MODE_TRUNCATE	= 2,
	};

	//----
	//ILog
	//----
	
	class LAMBDA_API ILog
	{
	public:
		LAMBDA_INTERFACE(ILog);

		virtual void Write(ELogVerbosity verbosity, const char* pFormat, ...) = 0;
		virtual void WriteV(ELogVerbosity verbosity, const char* pFormat, va_list args) = 0;
		
		virtual void SetVerbosityFilter(ELogVerbosity verbosity) = 0;
		virtual void SetConsoleOutputEnabled(bool bConsoleOutput) = 0;
		virtual void SetFileOutputEnabled(bool bFileOutput) = 0;

		virtual const char* GetName() const = 0;
		virtual ELogVerbosity GetVerbosityFilter() const = 0;
	};

	//----
	//Log
	//----

	class Log : public ILog
	{
		friend class LogManager;
	public:
		/*ILog interface*/
		virtual void Write(ELogVerbosity verbosity, const char* pFormat, ...) override;
		virtual void WriteV(ELogVerbosity verbosity, const char* pFormat, va_list args) override;

		virtual void SetVerbosityFilter(ELogVerbosity filter) override;
		virtual void SetConsoleOutputEnabled(bool bConsoleOutput) override;
		virtual void SetFileOutputEnabled(bool bFileOutput) override;
		
		virtual const char* GetName() const override
		{
			return m_Name.c_str();
		}

		virtual ELogVerbosity GetVerbosityFilter() const override
		{
			return m_Filter;
		}
	private:
		Log(const char* pName, ELogMode mode, ELogVerbosity filter, bool bWriteConsole, bool bWriteFile);
		~Log();

		bool OpenFile();
		bool CloseFile();
	private:
		FILE* m_pLogFile;
		
		ELogMode m_Mode;
		ELogVerbosity m_Filter;
		
		std::string m_Name;
		std::string m_Filename;

		bool m_bConsoleOutput;
		bool m_bFileOutput;
	};
}