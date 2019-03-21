#pragma once
#include "Time.hpp"
#include <chrono>

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
		Time m_LastTime;
		Time m_CurrentTime;
		Time m_TotalTime;
		Time m_DeltaTime;
	};

	inline Clock::Clock()
		: m_LastTime(0),
		m_CurrentTime(0),
		m_TotalTime(0),
		m_DeltaTime(0)
	{
	}

	inline Clock::Clock(Clock&& other)
		: m_LastTime(other.m_LastTime),
		m_CurrentTime(other.m_CurrentTime),
		m_TotalTime(other.m_TotalTime),
		m_DeltaTime(other.m_DeltaTime)
	{
	}

	inline Clock::Clock(const Clock& other)
		: m_LastTime(other.m_LastTime),
		m_CurrentTime(other.m_CurrentTime),
		m_TotalTime(other.m_TotalTime),
		m_DeltaTime(other.m_DeltaTime)
	{
	}

	inline Clock::~Clock()
	{
	}

	inline void Clock::Tick()
	{
		m_CurrentTime = Time(static_cast<float>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
		m_DeltaTime = m_CurrentTime - m_LastTime;
		m_LastTime = m_CurrentTime;
		m_TotalTime += m_DeltaTime;
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