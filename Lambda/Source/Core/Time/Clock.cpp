#include "LambdaPch.h"

#include "Core/Time/Clock.h"
#include "Core/Platform.h"

namespace Lambda
{
	//------
	//Clock
	//------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	Clock::Clock()
		: m_Frequency(0),
		m_DeltaTime(0),
		m_TotalTime(0),
		m_LastTime(0)
	{
		m_Frequency = Platform::Time::QueryPerformanceFrequency();
		Tick();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	Clock::Clock(Clock&& other) noexcept
		: m_Frequency(other.m_Frequency),
		m_LastTime(other.m_LastTime),
		m_TotalTime(other.m_TotalTime),
		m_DeltaTime(other.m_DeltaTime)
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	Clock::Clock(const Clock& other) noexcept
		: m_Frequency(other.m_Frequency),
		m_LastTime(other.m_LastTime),
		m_TotalTime(other.m_TotalTime),
		m_DeltaTime(other.m_DeltaTime)
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	Clock& Clock::operator=(Clock&& other) noexcept
	{
		if (this != &other)
		{
			m_Frequency = other.m_Frequency;
			m_DeltaTime = other.m_DeltaTime;
			m_TotalTime = other.m_TotalTime;
			m_LastTime	= other.m_LastTime;
			other.m_Frequency = 0;
			other.m_DeltaTime = 0;
			other.m_TotalTime = 0;
			other.m_LastTime  = 0;
		}

		return *this;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	Clock& Clock::operator=(const Clock& other) noexcept
	{
		if (this != &other)
		{
			m_Frequency = other.m_Frequency;
			m_DeltaTime = other.m_DeltaTime;
			m_TotalTime = other.m_TotalTime;
			m_LastTime	= other.m_LastTime;
		}

		return *this;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Clock::Tick()
	{
		//Get current time
		uint64 now = Platform::Time::QueryPerformanceCounter();
		Time currentTime = Time(now / m_Frequency);

		//Update delta- and totaltime
		m_DeltaTime = currentTime - m_LastTime;
		m_LastTime	= currentTime;
		m_TotalTime += m_DeltaTime;
	}
}
