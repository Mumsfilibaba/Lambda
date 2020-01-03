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
	class ILog
	{
	public:
		LAMBDA_INTERFACE(ILog);

		inline void Log(ELogVerbosity verbosity, const char* pFormat, ...)
		{
			va_list args;
			va_start(args, pFormat);
			ILog::Log(verbosity, pFormat, args);
			va_end(args);
		}

		virtual void Log(ELogVerbosity verbosity, const char* pFormat, va_list args) = 0;
		
		virtual void SetVerbosityFilter(ELogVerbosity verbosity) = 0;
		virtual void SetConsoleOutputEnabled(bool bConsoleOutput) = 0;
		virtual void SetFileOutputEnabled(bool bFileOutput) = 0;

		virtual const char* GetName() const = 0;
		virtual ELogVerbosity GetVerbosityFilter() const = 0;

		virtual void Flush() = 0;
	};

	//----
	//CLog
	//----
	class CLog : public ILog
	{
	public:
		CLog(const char* pName, ELogMode mode, ELogVerbosity filter);
		~CLog();

		/*ILog interface*/
		virtual void Log(ELogVerbosity verbosity, const char* pFormat, va_list args) override;

		virtual void SetVerbosityFilter(ELogVerbosity filter) override;
		virtual void SetConsoleOutputEnabled(bool bConsoleOutput) override;
		virtual void SetFileOutputEnabled(bool bFileOutput) override;

		virtual void Flush() override;
		
		/*////////////////////////////////////////*/
		virtual const char* GetName() const override
		{
			return m_Name.c_str();
		}

		/*/////////////////////////////////////////////////////*/
		virtual ELogVerbosity GetVerbosityFilter() const override
		{
			return m_Filter;
		}
	private:
		/*CLog Interface*/
		bool OpenFile();
		bool CloseFile();
	private:
		FILE* m_pLogFile;
		
		ELogMode m_Mode;
		ELogVerbosity m_Filter;
		
		std::string m_Name;
		std::string m_Buffer;

		bool m_bConsoleOutput;
		bool m_bFileOutput;
	};
}