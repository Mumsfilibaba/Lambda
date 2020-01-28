#pragma once
#include "LambdaCore.h"

#include "Time/Clock.h"

namespace Lambda
{
	class LAMBDA_API CEngineLoop final
	{
	public:
        CEngineLoop();
        ~CEngineLoop() = default;

        LAMBDA_DECL_NO_COPY(CEngineLoop);

        void PreInit();
        void Init();
        void Start();
        void Tick();
        void Terminate();
        void Release();
        void PostRelease();
        bool IsRunning() const { return m_bIsRunning; }
	private:
        CClock m_Frameclock;
        bool m_bIsRunning;
	};

    LAMBDA_API extern CEngineLoop g_EngineLoop;
}
