#pragma once
#include "CTime.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#if !defined(WIN32_LEAN_AND_MEAN)    
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <windows.h>
#else
    #include <chrono>
#endif

namespace Lambda
{
    //------
    //CClock
    //------
    
	class CClock
	{
	public:
		_forceinline CClock()
			: m_Frequency(0),
			m_LastTime(0),
			m_TotalTime(0),
			m_DeltaTime(0)
		{
#if defined(LAMBDA_PLAT_WINDOWS)
			QueryPerformanceFrequency((LARGE_INTEGER*)&m_Frequency);
#elif defined(LAMBDA_PLAT_MACOS)
			//Constructor is empty on macOS
#else
#error Clock::Clock not defined
#endif
			//Make first tick
			Tick();
		}


		_forceinline CClock(CClock&& other)
			: m_Frequency(other.m_Frequency),
			m_LastTime(other.m_LastTime),
			m_TotalTime(other.m_TotalTime),
			m_DeltaTime(other.m_DeltaTime)
		{
		}


		_forceinline CClock(const CClock& other)
			: m_Frequency(other.m_Frequency),
			m_LastTime(other.m_LastTime),
			m_TotalTime(other.m_TotalTime),
			m_DeltaTime(other.m_DeltaTime)
		{
		}
		~CClock() = default;


		_forceinline void Tick()
		{
#if defined(LAMBDA_PLAT_WINDOWS)
			//Get current time
			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);
			Timestep currentTime = Timestep((now.QuadPart * 1000000000) / m_Frequency);

			//Update delta- and totaltime
			m_DeltaTime = currentTime - m_LastTime;
			m_LastTime = currentTime;
			m_TotalTime += m_DeltaTime;
#elif defined(LAMBDA_PLAT_MACOS)
			//Get current time
			auto now = std::chrono::high_resolution_clock::now();
			auto duration = now.time_since_epoch();
			auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
			CTime currentTime = CTime::NanoSeconds(nanos);

			//Update delta- and totaltime
			m_DeltaTime = currentTime - m_LastTime;
			m_LastTime = currentTime;
			m_TotalTime += m_DeltaTime;
#else
    #error Clock::Tick not defined
#endif
		}


		_forceinline void Reset()
		{
			m_DeltaTime = CTime(0);
			m_TotalTime = CTime(0);
		}


		_forceinline CTime GetDeltaTime() const
		{
			return m_DeltaTime;
		}


		_forceinline CTime GetTotalTime() const
		{
			return m_TotalTime;
		}
	private:
		uint64 m_Frequency;
		CTime m_LastTime;
		CTime m_TotalTime;
		CTime m_DeltaTime;
	};
}
