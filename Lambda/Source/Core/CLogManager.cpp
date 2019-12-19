#include "LambdaPch.h"
#include "Core/CLogManager.h"

namespace Lambda
{
	//-----------
	//CLogManager
	//-----------

	template<>
	CLogManager* CSingleton<CLogManager>::s_pInstance = nullptr;

	CLogManager::CLogManager()
		: m_ChannelFilter(LOG_CHANNEL_ALL_CHANNELS)
	{
	}


	void CLogManager::Print(ELogChannel channel, ELogSeverity severity, const char* pFormat, ...)
	{
		//Filter the channel
		if ((m_ChannelFilter & channel) == 0)
		{
			return;
		}


		//Get severity as string
		const char* pSeverity = nullptr;
		EConsoleColor color = EConsoleColor::CONSOLE_COLOR_GREEN;
		if (severity == LOG_SEVERITY_INFO)
		{
			pSeverity = "INFO";
		}
		else if (severity == LOG_SEVERITY_MESSAGE)
		{
			pSeverity = "MESSAGE";
		}
		else if (severity == LOG_SEVERITY_WARNING)
		{
			pSeverity = "WARNING";
			color = EConsoleColor::CONSOLE_COLOR_YELLOW;
		}
		else if (severity == LOG_SEVERITY_ERROR)
		{
			pSeverity = "ERROR";
			color = EConsoleColor::CONSOLE_COLOR_RED;
		}


		//Get channel as string
		const char* pChannel = nullptr;
		if (channel == LOG_CHANNEL_ENGINE)
		{
			pChannel = "LAMBDA ENGINE";
		}
		else if (channel == LOG_CHANNEL_ENVIRONMENT)
		{
			pChannel = "ENVIRONMENT";
		}
		else if (channel == LOG_CHANNEL_RENDERER)
		{
			pChannel = "RENDERER";
		}
		else if (channel == LOG_CHANNEL_RENDER_API)
		{
			pChannel = "RENDER API";
		}
		else
		{
			pChannel = "";
		}

		//Print message into buffer
		constexpr uint32 MAX_LOG_BUFFER = 1024;
		static char s_Buffer[MAX_LOG_BUFFER];

		va_list args;
		va_start(args, pFormat);
		vsnprintf(s_Buffer, MAX_LOG_BUFFER, pFormat, args);
		va_end(args);

		//Print to the host
        LSetConsoleColor(color);
		if (channel == LOG_CHANNEL_ALL_CHANNELS)
		{
			LPrintConsole("[%s]: %s", pSeverity, s_Buffer);
		}
		else
		{
			LPrintConsole("[%s]: [%s] - %s", pSeverity, pChannel, s_Buffer);
		}
		LSetConsoleColor(EConsoleColor::CONSOLE_COLOR_WHITE);
	}


	void CLogManager::Release()
	{
		delete this;
	}
}
