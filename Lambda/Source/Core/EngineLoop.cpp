#include "LambdaPch.h"

#include "Core/EngineLoop.h"

#include "Platform/PlatformApplication.h"

namespace Lambda
{
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
		PlatformApplication::PollEvents();
		m_Frameclock.Tick();
	}

	void CEngineLoop::Release()
	{
		delete this;
	}
}