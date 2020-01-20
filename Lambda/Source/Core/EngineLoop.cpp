#include "LambdaPch.h"

#include "Core/EngineLoop.h"

#include "Platform/PlatformApplication.h"

namespace Lambda
{
	CEngineLoop* CEngineLoop::s_EngineLoop = nullptr;

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
		Platform::Application::PollMessages();
		m_Frameclock.Tick();
	}

	void CEngineLoop::Release()
	{
		delete this;
	}
}