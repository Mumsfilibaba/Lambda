#include <LambdaPch.h>
#include <System/Application.h>
#include <System/Clock.h>
#include <System/Log.h>
#include <System/Input.h>

namespace Lambda
{
	Application* Application::s_pInstance = nullptr;

	Application::Application()
		: m_pWindow(nullptr),
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

			Input::IsKeyUp(KEY_A);
		}

		InternalOnRelease();
		return 0;
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
			m_pWindow->SetEventCallback(EventDispatcher::OnEvent);
		}
	}

	void Application::Quit()
	{
		m_Running = false;
	}

	Application& Application::GetInstance()
	{
		assert(s_pInstance != nullptr);
		return *s_pInstance;
	}

	bool Application::OnEvent(const Event& event)
	{
		if (event.Type == EVENT_TYPE_WINDOW_CLOSED)
		{
			Application::GetInstance().Quit();
			LOG_DEBUG_INFO("Window closed");
			return true;
		}

		return false;
	}
	
	void Application::InternalOnLoad()
	{
		OnLoad();
	}
	
	void Application::InternalOnUpdate(Time dt)
	{
		m_pWindow->OnUpdate();

		OnUpdate(dt);
	}

	void Application::InternalOnRelease()
	{
		OnRelease();

		DeleteSafe(m_pWindow);
	}
}