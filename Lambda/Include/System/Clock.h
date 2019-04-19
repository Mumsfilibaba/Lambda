#pragma once
#include "Time.hpp"
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace Lambda
{
	class Clock
	{
	public:
		Clock();
		Clock(Clock&& other);
		Clock(const Clock& other);
		~Clock();

		void Tick();
		void Reset();

		Time GetDeltaTime() const;
		Time GetTotalTime() const;

	private:
		uint64 m_Frequency;
		Time m_LastTime;
		Time m_TotalTime;
		Time m_DeltaTime;
	};

	inline Clock::Clock()
		: m_Frequency(0),
		m_LastTime(0),
		m_TotalTime(0),
		m_DeltaTime(0)
	{
#if defined(LAMBDA_PLAT_WINDOWS)
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_Frequency);
#else
#error Clock::Clock not defined
#endif
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


	inline Clock::~Clock()
	{
	}


	inline void Clock::Tick()
	{
#if defined(LAMBDA_PLAT_WINDOWS)
		uint64 now = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&now);
		Time currentTime = Time((now * 1000000000) / m_Frequency);

		m_DeltaTime = currentTime - m_LastTime;
		m_LastTime = currentTime;
		m_TotalTime += m_DeltaTime;
#else
#error Clock::Tick not defined
#endif
	}


	inline void Clock::Reset()
	{
		m_DeltaTime = Time(0);
		m_TotalTime = Time(0);
	}


	inline Time Clock::GetDeltaTime() const
	{
		return m_DeltaTime;
	}


	inline Time Clock::GetTotalTime() const
	{
		return m_TotalTime;
	}
}