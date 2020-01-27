#include "LambdaPch.h"

#include "Core/Engine.h"
#include "Core/EngineLoop.h"

#include "Core/Log/EngineLog.h"

#include "Core/Debug/IConsoleOutput.h"

#include "Platform/Platform.h"

#include <chrono>

namespace Lambda
{
    LAMBDA_API CEngineLoop g_EngineLoop;
    LAMBDA_API IConsoleOutput* g_pConsoleOutput = nullptr;

	CEngineLoop::CEngineLoop()
		: m_bIsRunning(false),
		m_Frameclock()
	{
	}

    void CEngineLoop::PreInit()
    {
        //Init console
        g_pConsoleOutput = Platform::CreateConsoleOutput();
        if (g_pConsoleOutput)
        {
            g_pConsoleOutput->SetTitle("Lambda Engine Debug Console");
            g_pConsoleOutput->Show();
        }
        
        //Init log
        CEngineLog::Init();
        CEngineLog::GetLog().Enable(true);
        CEngineLog::GetCoreLog().Enable(true);

        LOG_CORE_INFO("Finished Pre-Init");
    }

	void CEngineLoop::Init()
	{
        m_bIsRunning = false;

        g_Engine.Init();
        LOG_CORE_INFO("Finished Init");
	}

	void CEngineLoop::Start()
	{
        LOG_CORE_INFO("Starting Engine");

        //Start engine
		m_bIsRunning = true;
        
        //Show window when engine is completly initialized
        g_Engine.GetWindow().Show();

        //Make first tick on frameclock - To prevent that the first frame gets huge values in dt
        m_Frameclock.Tick();
	}

	void CEngineLoop::Tick()
	{
		Platform::PollEvents();
		m_Frameclock.Tick();
	}

    void CEngineLoop::Terminate()
    {
        LOG_CORE_INFO("Terminating Engine");
        m_bIsRunning = false;
    }

	void CEngineLoop::Release()
	{
        LOG_CORE_INFO("Engine Release");
        g_Engine.Release();
	}

    void CEngineLoop::PostRelease()
    {
        LOG_CORE_INFO("Engine Post-Release");
        CEngineLog::Release();
    }
}
