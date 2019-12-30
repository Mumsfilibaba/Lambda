#include "LambdaPch.h"

#include "Core/Engine/Engine.h"
#include "Core/Engine/System.h"
#include "Core/Engine/Console.h"

#include "Core/Input/Input.h"

//----------------
//_CreateGameLayer
//----------------
extern Lambda::CLayer* (*_CreateGameLayer)(Lambda::IEngine*);

namespace Lambda
{
	//-------
	//IEngine
	//-------

	IEngine* IEngine::s_pEngineInstance = nullptr;
	
	IEngine* IEngine::Get()
	{
		LAMBDA_ASSERT_PRINT(s_pEngineInstance != nullptr, "Engine has not been initialized");
		return s_pEngineInstance;
	}


	bool IEngine::Initialize(const SEngineParams& engineParams)
	{
		s_pEngineInstance = DBG_NEW CEngine();
		if (s_pEngineInstance)
		{
			return s_pEngineInstance->InternalInit(engineParams);
		}

		return false;
	}


	//-------
	//CEngine
	//-------

	CEngine::CEngine()
		: IEngine()
	{
		//Init all pointers to nullptr
		m_pConsole	= nullptr;
		m_pSystem	= nullptr;
		m_pInput	= nullptr;

		//Setup default frametime
		m_Frametime.Timestep	  = CTime::Seconds(1.0f / 60.0f);
		m_Frametime.UpdateBacklog = CTime(0);

		//Setup enginestate
		m_State.bIsRunning = false;
		m_State.ExitCode = 0;
	}


	CEngine::~CEngine()
	{
		m_pInput->Release();
		m_pSystem->Release();
		m_pConsole->Release();
	}


	bool CEngine::InternalInit(const SEngineParams&)
	{
		//Create console
		IConsole* pConsole = IConsole::Create();
		m_pConsole = pConsole;

		pConsole->PrintLine("Test");
		pConsole->SetTextColor(EConsoleColor::CONSOLE_COLOR_GREEN);
		pConsole->PrintLine("test2 = %d", 5);
		pConsole->Clear();
		pConsole->Reset();
		pConsole->PrintLine("Test3");

		//Create system
		ISystem* pSystem = ISystem::Create();
		m_pSystem = pSystem;

		//Create inputcontroller
		IInput* pInput = IInput::Create();
		m_pInput = pInput;

		//Create gamelayer
		if (_CreateGameLayer)
		{
			_CreateGameLayer(this);
		}
		else
		{
			return false;
		}

		return true;
	}


	void CEngine::RunMainLoop() 
	{
		//Startup engine
		m_State.bIsRunning = true;

		//MainLoop
		while (m_State.bIsRunning)
		{
			int i = 0;
		}
	}


	void CEngine::Exit(int32 exitCode)
	{
		m_State.bIsRunning	= false;
		m_State.ExitCode	= exitCode;
	}


	void CEngine::Release()
	{
		delete this;
	}
}