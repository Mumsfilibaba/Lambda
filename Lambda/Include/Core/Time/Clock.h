#pragma once
#include "Time.h"

namespace Lambda
{
    //------
    //CClock
    //------

	class CClock
	{
	public:
		CClock();
		CClock(CClock&& other) noexcept;
		CClock(const CClock& other) noexcept;
		~CClock() = default;

		CClock& operator=(CClock&& other) noexcept;
		CClock& operator=(const CClock& other) noexcept;

		void Tick();

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
