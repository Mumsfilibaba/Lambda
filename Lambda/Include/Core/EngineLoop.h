#pragma once
#include "LambdaCore.h"

#include "Time/Clock.h"

namespace Lambda
{
	class LAMBDA_API CEngineLoop
	{
	public:
		bool IsRunning() const { return m_bIsRunning; }
		void Init();
		void Start();
		void Tick();
		void Release();

		static CEngineLoop& Get()
		{
			LAMBDA_ASSERT_PRINT(s_EngineLoop, "EngineLoop not initialized");
			return *s_EngineLoop;
		}

		static CEngineLoop* Create()
		{
			LAMBDA_ASSERT_PRINT(!s_EngineLoop, "EngineLoop already initialized");

			s_EngineLoop = DBG_NEW CEngineLoop();
			return s_EngineLoop;
		}
	private:
		CEngineLoop() = default;
		~CEngineLoop() = default;
	private:
		bool m_bIsRunning;
		CClock m_Frameclock;

		static CEngineLoop* s_EngineLoop;
	};
}