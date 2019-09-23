#include "LambdaPch.h"
#include "WindowsLog.h"

#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	//----
	//ILog
	//----

	ILog* ILog::Create()
	{
		return DBG_NEW WindowsLog();
	}
    
	//----------
	//WindowsLog
	//----------
    
	void WindowsLog::Print(LogSeverity severity, const char* pFormat, ...)
	{
		WORD white = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		WORD color = white;

		std::string message;
		if (severity == LOG_SEVERITY_INFO)
		{
			color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			message = "[INFO]: ";
		}
		else if (severity == LOG_SEVERITY_WARNING)
		{
			color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
			message = "[WARNING]: ";
		}
		else if (severity == LOG_SEVERITY_ERROR)
		{
			color = FOREGROUND_RED;
			message = "[ERROR]: ";
		}

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
        printf(message.c_str());
        
        va_list args;
		va_start(args, pFormat);
		vprintf(pFormat, args);
		va_end(args);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), white);
	}
}
#endif
