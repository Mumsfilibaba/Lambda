#pragma once
#include "LambdaCore.h"
#include "Utilities/Singleton.h"
#include <memory>

//Log
#if defined(LAMBDA_DEBUG) && !defined(LAMBDA_NO_LOGS)
	#define LOG_DEBUG(...) Lambda::LogManager::Get().Print(__VA_ARGS__)

	#define LOG_ENGINE_INFO(...)	Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_ENGINE, Lambda::LOG_SEVERITY_INFO, __VA_ARGS__)
	#define LOG_ENGINE_MESSAGE(...)	Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_ENGINE, Lambda::LOG_SEVERITY_MESSAGE, __VA_ARGS__)
	#define LOG_ENGINE_WARNING(...) Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_ENGINE, Lambda::LOG_SEVERITY_WARNING, __VA_ARGS__)
	#define LOG_ENGINE_ERROR(...)	Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_ENGINE, Lambda::LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK
	
	#define LOG_HOST_INFO(...)		Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_HOST, Lambda::LOG_SEVERITY_INFO, __VA_ARGS__)
	#define LOG_HOST_MESSAGE(...)	Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_HOST, Lambda::LOG_SEVERITY_MESSAGE, __VA_ARGS__)
	#define LOG_HOST_WARNING(...)	Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_HOST, Lambda::LOG_SEVERITY_WARNING, __VA_ARGS__)
	#define LOG_HOST_ERROR(...)		Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_HOST, Lambda::LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK
	
	#define LOG_RENDERER_INFO(...)		Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_RENDERER, Lambda::LOG_SEVERITY_INFO, __VA_ARGS__)
	#define LOG_RENDERER_MESSAGE(...)	Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_RENDERER, Lambda::LOG_SEVERITY_MESSAGE, __VA_ARGS__)
	#define LOG_RENDERER_WARNING(...)	Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_RENDERER, Lambda::LOG_SEVERITY_WARNING, __VA_ARGS__)
	#define LOG_RENDERER_ERROR(...)		Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_RENDERER, Lambda::LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK
	
	#define LOG_RENDER_API_INFO(...)	Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_RENDER_API, Lambda::LOG_SEVERITY_INFO, __VA_ARGS__)
	#define LOG_RENDER_API_MESSAGE(...)	Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_RENDER_API, Lambda::LOG_SEVERITY_MESSAGE, __VA_ARGS__)
	#define LOG_RENDER_API_WARNING(...) Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_RENDER_API, Lambda::LOG_SEVERITY_WARNING, __VA_ARGS__)
	#define LOG_RENDER_API_ERROR(...)	Lambda::LogManager::Get().Print(Lambda::LOG_CHANNEL_RENDER_API, Lambda::LOG_SEVERITY_ERROR, __VA_ARGS__); DEBUG_BREAK
#else
	#define LOG_DEBUG(...)

	#define LOG_ENGINE_INFO(...)
	#define LOG_ENGINE_MESSAGE(...)
	#define LOG_ENGINE_WARNING(...)
	#define LOG_ENGINE_ERROR(...)
	
	#define LOG_HOST_INFO(...)		
	#define LOG_HOST_MESSAGE(...)	
	#define LOG_HOST_WARNING(...)	
	#define LOG_HOST_ERROR(...)		
	
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
	#define LOG(...) Lambda::LogManager::Get().Print(__VA_ARGS__)
#else
	#define LOG(...)
#endif


//Asserts
#ifdef LAMBDA_DEBUG
	#define LAMBDA_ENABLE_ASSERTS
#endif

#ifdef LAMBDA_ENABLE_ASSERTS
	#define LAMBDA_ASSERT(x)			{ if(!(x)) { LOG_ENGINE_ERROR("Assertion Failed\n"); DEBUG_BREAK; } }
	#define LAMBDA_ASSERT_PRINT(x, ...) { if(!(x)) { LOG_ENGINE_ERROR(__VA_ARGS__); DEBUG_BREAK; } }
#else
	#define LAMBDA_ASSERT(x)		
	#define LAMBDA_ASSERT_PRINT(x, ...)
#endif


namespace Lambda
{
    //-----------
    //LogSeverity
    //-----------
    
	enum LogSeverity : uint32
	{
		LOG_SEVERITY_UNKNOWN    = 0,
		LOG_SEVERITY_INFO       = 1,
		LOG_SEVERITY_WARNING    = 2,
		LOG_SEVERITY_ERROR      = 3,
        LOG_SEVERITY_MESSAGE    = 4,
	};

	//----------
	//LogChannel
	//----------

	enum LogChannel : uint32
	{
		LOG_CHANNEL_UNKNOWN			= 0,
		LOG_CHANNEL_ENGINE			= (1 << 0),
		LOG_CHANNEL_HOST			= (1 << 1),
		LOG_CHANNEL_RENDERER		= (1 << 2),
		LOG_CHANNEL_RENDER_API		= (1 << 3),
		LOG_CHANNEL_ALL_CHANNELS	= 0xffffffff
	};

    //----------
    //LogManager
    //----------

	class LAMBDA_API LogManager : public Singleton<LogManager>
	{
	public:
		LogManager();
		~LogManager() = default;

		void Print(LogChannel channel, LogSeverity severity, const char* pFormat, ...);
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
