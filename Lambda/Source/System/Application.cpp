#include <LambdaPch.h>
#include <System/Application.h>
#include <System/Clock.h>
#include <System/Log.h>
#include <System/Input.h>
#include <System/JoystickManager.h>

namespace Lambda
{
	Application* Application::s_pInstance = nullptr;

	Application::Application()
		: m_pWindow(nullptr),
		m_pGraphicsContext(nullptr),
		m_Running(true)
	{
		assert(s_pInstance == nullptr);
		s_pInstance = this;

		Init();
	}

	Application::~Application()
	{
	}

	int32 Application::Run()
	{
		InternalOnLoad();

		Clock clock;
		while (m_Running)
		{
			clock.Tick();
			InternalOnUpdate(clock.GetDeltaTime());
		}

		InternalOnRelease();
		return m_ExitCode;
	}

	void Application::Init()
	{
		//Setup application layer
		{
			EventLayer applicationLayer = { Application::OnEvent, "ApplicationLayer" };
			EventDispatcher::PushEventLayer(applicationLayer);
		}

		//Create window
		{
			WindowDesc desc = {};
			desc.pTitle = "Lambda Engine";
			desc.Width = 1600;
			desc.Height = 900;

			m_pWindow = IWindow::Create(desc);
			m_pWindow->SetEventCallback(EventDispatcher::SendEvent);
		}

		//Create graphics context
		{
#if LAMBDA_DEBUG
			m_pGraphicsContext = IGraphicsContext::Create(m_pWindow, GRAPHICS_CONTEXT_FLAG_DEBUG);
#else
			m_pGraphicsContext = IGraphicsContext::Create(m_pWindow, GRAPHICS_CONTEXT_FLAG_NONE);
#endif

			EventLayer graphicsLayer = { IGraphicsContext::OnEvent, "GraphicsLayer" };
			EventDispatcher::PushEventLayer(graphicsLayer);
		}
	}

	void Application::Quit(int32 exitCode)
	{
		m_Running = false;
		m_ExitCode = exitCode;
	}

	Application& Application::GetInstance()
	{
		assert(s_pInstance != nullptr);
		return *s_pInstance;
	}
	
	void Application::InternalOnLoad()
	{
		OnLoad();
	}
	
	void Application::InternalOnUpdate(Time dt)
	{
		m_pWindow->OnUpdate();
		JoystickManager::OnUpdate();
		OnUpdate(dt);
	}

	void Application::InternalOnRelease()
	{
		OnRelease();

		SafeDelete(m_pWindow);
		SafeRelease(m_pGraphicsContext);
	}

	bool Application::OnEvent(const Event& event)
	{
		if (event.Type == EVENT_TYPE_WINDOW_CLOSED)
		{
			Application::GetInstance().Quit(0);
			LOG_DEBUG_INFO("Window closed");
			return true;
		}
		else if (event.Type == EVENT_TYPE_KEYDOWN)
		{
			if (event.KeyEvent.KeyCode == KEY_ESCAPE)
			{
				Application::GetInstance().Quit(0);
				return true;
			}
		}

		return false;
	}
}