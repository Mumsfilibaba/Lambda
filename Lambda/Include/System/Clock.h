#pragma once
#include <LambdaPch.h>
#include "Time.hpp"

namespace Lambda
{
	class LAMBDA_API Clock
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
}