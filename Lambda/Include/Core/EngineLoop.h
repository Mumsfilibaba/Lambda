#pragma once
#include "LambdaCore.h"

#include "Time/Clock.h"

namespace Lambda
{
	class LAMBDA_API CEngineLoop
	{
	public:
        CEngineLoop();
        ~CEngineLoop() = default;

        void Init();
        void Start();
        void Tick();
        void Release();
        bool IsRunning() const { return m_bIsRunning; }
	private:
		bool m_bIsRunning;
		CClock m_Frameclock;
	};

    extern CEngineLoop g_EngineLoop;
}
