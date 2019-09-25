#pragma once
#include "Timestep.h"
#if defined(LAMBDA_PLAT_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <chrono>
#endif

namespace Lambda
{
    //-----
    //Clock
    //-----
    
	class Clock
	{
	public:
		Clock();
		Clock(Clock&& other);
		Clock(const Clock& other);
		~Clock() = default;

		void Tick();
		void Reset();
		Timestep GetDeltaTime() const;
		Timestep GetTotalTime() const;
	private:
		uint64 m_Frequency;
		Timestep m_LastTime;
		Timestep m_TotalTime;
		Timestep m_DeltaTime;
	};

    
	inline Clock::Clock()
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

	inline Clock::Clock(Clock&& other)
		: m_Frequency(other.m_Frequency),
		m_LastTime(other.m_LastTime),
		m_TotalTime(other.m_TotalTime),
		m_DeltaTime(other.m_DeltaTime)
	{
	}


	inline Clock::Clock(const Clock& other)
		: m_Frequency(other.m_Frequency),
		m_LastTime(other.m_LastTime),
		m_TotalTime(other.m_TotalTime),
		m_DeltaTime(other.m_DeltaTime)
	{
	}


	inline void Clock::Tick()
	{
#if defined(LAMBDA_PLAT_WINDOWS)
		//Get current time
        uint64 now = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&now);
		Timestep currentTime = Timestep((now * 1000000000) / m_Frequency);

        //Update delta- and totaltime
		m_DeltaTime = currentTime - m_LastTime;
		m_LastTime = currentTime;
		m_TotalTime += m_DeltaTime;
#elif defined(LAMBDA_PLAT_MACOS)
        //Get current time
        auto now        = std::chrono::high_resolution_clock::now();
        auto duration   = now.time_since_epoch();
        auto nanos      = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
        Timestep currentTime = Timestep::NanoSeconds(nanos);
        
        //Update delta- and totaltime
        m_DeltaTime = currentTime - m_LastTime;
        m_LastTime = currentTime;
        m_TotalTime += m_DeltaTime;
#else
    #error Clock::Tick not defined
#endif
	}


	inline void Clock::Reset()
	{
		m_DeltaTime = Timestep(0);
		m_TotalTime = Timestep(0);
	}


	inline Timestep Clock::GetDeltaTime() const
	{
		return m_DeltaTime;
	}


	inline Timestep Clock::GetTotalTime() const
	{
		return m_TotalTime;
	}
}
