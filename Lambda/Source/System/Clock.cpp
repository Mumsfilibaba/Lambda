#include <LambdaPch.h>
#include <System/Clock.h>

namespace Lambda
{
	Clock::Clock()
		: m_LastTime(0),
		m_CurrentTime(0),
		m_TotalTime(0),
		m_DeltaTime(0)
	{
	}

	Clock::Clock(Clock&& other)
		: m_LastTime(other.m_LastTime),
		m_CurrentTime(other.m_CurrentTime),
		m_TotalTime(other.m_TotalTime),
		m_DeltaTime(other.m_DeltaTime)
	{
	}

	Clock::Clock(const Clock& other)
		: m_LastTime(other.m_LastTime),
		m_CurrentTime(other.m_CurrentTime),
		m_TotalTime(other.m_TotalTime),
		m_DeltaTime(other.m_DeltaTime)
	{
	}

	Clock::~Clock()
	{
	}

	void Clock::Tick()
	{
		m_CurrentTime = Time(static_cast<float>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
		m_DeltaTime = m_CurrentTime - m_LastTime;
		m_LastTime = m_CurrentTime;
		m_TotalTime += m_DeltaTime;
	}

	void Clock::Reset()
	{
		m_DeltaTime = Time(0);
		m_TotalTime = Time(0);
	}
	
	Time Clock::GetDeltaTime() const
	{
		return m_DeltaTime;
	}

	Time Clock::GetTotalTime() const
	{
		return m_TotalTime;
	}
}
