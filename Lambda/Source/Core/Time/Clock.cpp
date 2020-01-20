#include "LambdaPch.h"

#include "Core/Time/Clock.h"

#include "Platform/PlatformTime.h"

namespace Lambda
{
	CClock::CClock()
		: m_DeltaTime(0),
		m_TotalTime(0),
		m_LastTime(0)
	{
		Tick();
	}

	CClock::CClock(CClock&& other) noexcept
		: m_LastTime(other.m_LastTime),
		m_TotalTime(other.m_TotalTime),
		m_DeltaTime(other.m_DeltaTime)
	{
	}

	CClock::CClock(const CClock& other) noexcept
		: m_LastTime(other.m_LastTime),
		m_TotalTime(other.m_TotalTime),
		m_DeltaTime(other.m_DeltaTime)
	{
	}

	CClock& CClock::operator=(CClock&& other) noexcept
	{
		if (this != &other)
		{
			m_DeltaTime = other.m_DeltaTime;
			m_TotalTime = other.m_TotalTime;
			m_LastTime	= other.m_LastTime;
			other.m_DeltaTime = 0;
			other.m_TotalTime = 0;
			other.m_LastTime  = 0;
		}

		return *this;
	}

	CClock& CClock::operator=(const CClock& other) noexcept
	{
		if (this != &other)
		{
			m_DeltaTime = other.m_DeltaTime;
			m_TotalTime = other.m_TotalTime;
			m_LastTime	= other.m_LastTime;
		}

		return *this;
	}

	void CClock::Tick()
	{
		//Get current time
		uint64 ns = Platform::Time::Nanoseconds();
		CTimestep now = CTimestep(ns);

		//Update delta- and totaltime
		m_DeltaTime = now - m_LastTime;
		m_LastTime	= now;
		m_TotalTime += m_DeltaTime;
	}
}
