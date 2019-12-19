#pragma once
#include "CSingleton.h"
#include <memory>

//Log
#if defined(LAMBDA_DEBUG) && !defined(LAMBDA_NO_LOGS)
	#define LOG_DEBUG(...) Lambda::CLogManager::Get().Print(__VA_ARGS__)

	#define LOG_ENGINE_INFO(...)	Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_ENGINE, Lambda::LOG_SEVERITY_INFO, __VA_ARGS__)
	#define LOG_ENGINE_MESSAGE(...)	Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_ENGINE, Lambda::LOG_SEVERITY_MESSAGE, __VA_ARGS__)
	#define LOG_ENGINE_WARNING(...) Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_ENGINE, Lambda::LOG_SEVERITY_WARNING, __VA_ARGS__)
	#define LOG_ENGINE_ERROR(...)	Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_ENGINE, Lambda::LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK
	
	#define LOG_ENVIRONMENT_INFO(...)		Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_ENVIRONMENT, Lambda::LOG_SEVERITY_INFO, __VA_ARGS__)
	#define LOG_ENVIRONMENT_MESSAGE(...)	Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_ENVIRONMENT, Lambda::LOG_SEVERITY_MESSAGE, __VA_ARGS__)
	#define LOG_ENVIRONMENT_WARNING(...)	Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_ENVIRONMENT, Lambda::LOG_SEVERITY_WARNING, __VA_ARGS__)
	#define LOG_ENVIRONMENT_ERROR(...)		Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_ENVIRONMENT, Lambda::LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK
	
	#define LOG_RENDERER_INFO(...)		Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_RENDERER, Lambda::LOG_SEVERITY_INFO, __VA_ARGS__)
	#define LOG_RENDERER_MESSAGE(...)	Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_RENDERER, Lambda::LOG_SEVERITY_MESSAGE, __VA_ARGS__)
	#define LOG_RENDERER_WARNING(...)	Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_RENDERER, Lambda::LOG_SEVERITY_WARNING, __VA_ARGS__)
	#define LOG_RENDERER_ERROR(...)		Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_RENDERER, Lambda::LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK
	
	#define LOG_RENDER_API_INFO(...)	Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_RENDER_API, Lambda::LOG_SEVERITY_INFO, __VA_ARGS__)
	#define LOG_RENDER_API_MESSAGE(...)	Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_RENDER_API, Lambda::LOG_SEVERITY_MESSAGE, __VA_ARGS__)
	#define LOG_RENDER_API_WARNING(...) Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_RENDER_API, Lambda::LOG_SEVERITY_WARNING, __VA_ARGS__)
	#define LOG_RENDER_API_ERROR(...)	Lambda::CLogManager::Get().Print(Lambda::LOG_CHANNEL_RENDER_API, Lambda::LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK
#else
	#define LOG_DEBUG(...)

	#define LOG_ENGINE_INFO(...)
	#define LOG_ENGINE_MESSAGE(...)
	#define LOG_ENGINE_WARNING(...)
	#define LOG_ENGINE_ERROR(...)
	
	#define LOG_ENVIRONMENT_INFO(...)
	#define LOG_ENVIRONMENT_MESSAGE(...)
	#define LOG_ENVIRONMENT_WARNING(...)
	#define LOG_ENVIRONMENT_ERROR(...)
	
	#define LOG_RENDERER_INFO(...)		
	#define LOG_RENDERER_MESSAGE(...)	
	#define LOG_RENDERER_WARNING(...)	
	#define LOG_RENDERER_ERROR(...)		
	
	#define LOG_RENDER_API_INFO(...)	
	#define LOG_RENDER_API_MESSAGE(...)	
	#define LOG_RENDER_API_WARNING(...) 
	#define LOG_RENDER_API_ERROR(...)	
#endif

#if !defined(LAMBDA_NO_LOGS)
	#define LOG(...) Lambda::CLogManager::Get().Print(__VA_ARGS__)
#else
	#define LOG(...)
#endif

namespace Lambda
{
    //------------
    //ELogSeverity
    //------------
    
	enum ELogSeverity : uint8
	{
		LOG_SEVERITY_UNKNOWN    = 0,
		LOG_SEVERITY_INFO       = 1,
		LOG_SEVERITY_WARNING    = 2,
		LOG_SEVERITY_ERROR      = 3,
        LOG_SEVERITY_MESSAGE    = 4,
	};

	//-----------
	//ELogChannel
	//-----------

	enum ELogChannel : uint32
	{
		LOG_CHANNEL_UNKNOWN			= 0,
		LOG_CHANNEL_ENGINE			= (1 << 0),
		LOG_CHANNEL_ENVIRONMENT		= (1 << 1),
		LOG_CHANNEL_RENDERER		= (1 << 2),
		LOG_CHANNEL_RENDER_API		= (1 << 3),
		LOG_CHANNEL_ALL_CHANNELS	= 0xffffffff
	};

    //-----------
    //CLogManager
    //-----------

	class LAMBDA_API CLogManager : public CSingleton<CLogManager>
	{
	public:
		CLogManager();
		~CLogManager() = default;

        LAMBDA_NO_COPY(CLogManager);
        
		void Print(ELogChannel channel, ELogSeverity severity, const char* pFormat, ...);
		void Release();

		_forceinline void SetChannelFilter(uint32 channelFilter)
		{
			m_ChannelFilter = channelFilter;
		}


		_forceinline uint32 GetChannelFilter() const
		{
			return m_ChannelFilter;
		}
	private:
		uint32 m_ChannelFilter;
	};
}
