#pragma once
#include "Timestep.h"

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
			m_DeltaTime = CTimestep(0);
			m_TotalTime = CTimestep(0);
		}

		_forceinline const CTimestep& GetDeltaTime() const
		{
			return m_DeltaTime;
		}

		_forceinline const CTimestep& GetTotalTime() const
		{
			return m_TotalTime;
		}
	private:
		CTimestep m_LastTime;
		CTimestep m_TotalTime;
		CTimestep m_DeltaTime;
	};
}