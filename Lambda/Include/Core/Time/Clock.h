#pragma once
#include "Timestamp.h"

namespace Lambda
{
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
			m_DeltaTime = CTimestamp(0);
			m_TotalTime = CTimestamp(0);
		}

		_forceinline const CTimestamp& GetDeltaTime() const
		{
			return m_DeltaTime;
		}

		_forceinline const CTimestamp& GetTotalTime() const
		{
			return m_TotalTime;
		}

		static CTimestamp Now();
	private:
		CTimestamp m_LastTime;
		CTimestamp m_TotalTime;
		CTimestamp m_DeltaTime;
	};
}