#include "LambdaPch.h"

#include "Core/Time/CClock.h"
#include "Core/Platform.h"

namespace Lambda
{
	//------
	//CClock
	//------

	/*///////////////*/
	CClock::CClock()
		: m_Frequency(0),
		m_DeltaTime(0),
		m_TotalTime(0),
		m_LastTime(0)
	{
		m_Frequency = Platform::QueryPerformanceFrequency();
		Tick();
	}

	/*//////////////////////////*/
	CClock::CClock(CClock&& other)
		: m_Frequency(other.m_Frequency),
		m_LastTime(other.m_LastTime),
		m_TotalTime(other.m_TotalTime),
		m_DeltaTime(other.m_DeltaTime)
	{
	}

	/*///////////////////////////////*/
	CClock::CClock(const CClock& other)
		: m_Frequency(other.m_Frequency),
		m_LastTime(other.m_LastTime),
		m_TotalTime(other.m_TotalTime),
		m_DeltaTime(other.m_DeltaTime)
	{
	}

	/*/////////////////////////////////////*/
	CClock& CClock::operator=(CClock&& other)
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

	/*//////////////////////////////////////////*/
	CClock& CClock::operator=(const CClock& other)
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

	/*///////////////*/
	void CClock::Tick()
	{
		//Get current time
		uint64 now = Platform::QueryPerformanceCounter();
		CTime currentTime = CTime((now * 1000000000) / m_Frequency);

		//Update delta- and totaltime
		m_DeltaTime = currentTime - m_LastTime;
		m_LastTime	= currentTime;
		m_TotalTime += m_DeltaTime;
	}
}
