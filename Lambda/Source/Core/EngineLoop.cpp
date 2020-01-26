#include "LambdaPch.h"

#include "Core/EngineLoop.h"
#include "Platform/Platform.h"

#include <chrono>

namespace Lambda
{
    CEngineLoop g_EngineLoop;

	CEngineLoop::CEngineLoop()
		: m_bIsRunning(false),
		m_Frameclock()
	{
	}

	void CEngineLoop::Init()
	{
		m_bIsRunning = false;
	}

	void CEngineLoop::Start()
	{
		m_bIsRunning = true;
	}

	void CEngineLoop::Tick()
	{
		Platform::PollEvents();
		m_Frameclock.Tick();
	}

	void CEngineLoop::Release()
	{
	}
}
