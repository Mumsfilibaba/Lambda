#pragma once
#include "Time.h"

namespace Lambda
{
    //-----
    //Clock
    //-----

	class Clock
	{
	public:
		Clock();
		Clock(Clock&& other) noexcept;
		Clock(const Clock& other) noexcept;
		~Clock() = default;

		Clock& operator=(Clock&& other) noexcept;
		Clock& operator=(const Clock& other) noexcept;

		void Tick();

		/////////////////////////
		_forceinline void Reset()
		{
			m_DeltaTime = Time(0);
			m_TotalTime = Time(0);
		}

		///////////////////////////////////////
		_forceinline Time GetDeltaTime() const
		{
			return m_DeltaTime;
		}

		///////////////////////////////////////
		_forceinline Time GetTotalTime() const
		{
			return m_TotalTime;
		}
	private:
		uint64 m_Frequency;
		Time m_LastTime;
		Time m_TotalTime;
		Time m_DeltaTime;
	};
}
