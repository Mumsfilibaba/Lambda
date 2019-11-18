#include "LambdaPch.h"
#include "Core/Host.h"
#include "Core/LogManager.h"

namespace Lambda
{
	//----------
	//LogManager
	//----------

	LogManager::LogManager()
		: m_ChannelFilter(LOG_CHANNEL_ALL_CHANNELS)
	{
	}


	void LogManager::Print(LogChannel channel, LogSeverity severity, const char* pFormat, ...)
	{
		//Filter the channel
		if (m_ChannelFilter & channel == 0)
		{
			return;
		}

		//Get severity as string
		const char* pSeverity = nullptr;
		if (severity == LOG_SEVERITY_INFO)
			pSeverity = "INFO";
		else if (severity == LOG_SEVERITY_MESSAGE)
			pSeverity = "MESSAGE";
		else if (severity == LOG_SEVERITY_WARNING)
			pSeverity = "WARNING";
		else if (severity == LOG_SEVERITY_ERROR)
			pSeverity = "ERROR";

		//Get channel as string
		const char* pChannel = nullptr;
		if (channel == LOG_CHANNEL_ENGINE)
			pChannel = "LAMBDA ENGINE";
		else if (channel == LOG_CHANNEL_HOST)
			pChannel = "HOST";
		else if (channel == LOG_CHANNEL_RENDERER)
			pChannel = "RENDERER";
		else if (channel == LOG_CHANNEL_RENDER_API)
			pChannel = "RENDER API";
		else
			pChannel = "";

		//Print message into buffer
		constexpr uint32 MAX_LOG_BUFFER = 1024;
		static char s_Buffer[MAX_LOG_BUFFER];

		va_list args;
		va_start(args, pFormat);
		vsnprintf(s_Buffer, MAX_LOG_BUFFER, pFormat, args);
		va_end(args);

		//Print to the host
		Host::Get().PrintF("[%s]: [%s] - %s", pSeverity, pChannel, s_Buffer);
	}


	void LogManager::Release()
	{
		delete this;
	}
}