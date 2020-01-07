#include "LambdaPch.h"

#include "Core//Platform.h"

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

	IEngine* IEngine::s_pInstance = nullptr;
	
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	IEngine* IEngine::Get()
	{
		LAMBDA_ASSERT_PRINT(s_pInstance != nullptr, "Engine has not been initialized");
		return s_pInstance;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool IEngine::Create(const SEngineParams& engineParams)
	{
		LAMBDA_ASSERT_PRINT(s_pInstance == nullptr, "Engine can only be created once");

		s_pInstance = DBG_NEW CEngine();
		if (s_pInstance)
		{
			return s_pInstance->Initialize(engineParams);
		}

		return false;
	}


	//-------
	//CEngine
	//-------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CEngine::CEngine()
		: IEngine()
	{
		//Init all pointers to nullptr
		m_pSystem	= nullptr;

		//Setup default frametime
		m_Frametime.Timestep	  = CTime::Seconds(1.0f / 60.0f);
		m_Frametime.UpdateBacklog = CTime(0);

		//Setup enginestate
		m_State.bIsRunning = false;
		m_State.ExitCode = 0;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CEngine::~CEngine()
	{
		CInput::Release();
		m_pSystem->Release();
		CConsole::Release();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool CEngine::Initialize(const SEngineParams&)
	{
		//Create console
		if (!CConsole::Initialize())
		{
			return false;
		}
        
		//Create system
		ISystem* pSystem = ISystem::Create(this);
		if (pSystem)
		{
            //LAMBDA_ASSERT_PRINT(false, "Assertion Test");
            
			m_pSystem = pSystem;
			if (!m_pSystem->Initialize())
			{
				return false;
			}

			if (!m_pSystem->CreateWindow("Lambda Engine", 1440, 900))
			{
				return false;
			}

			//Add this to the system event listeners
			m_pSystem->AddEventListener(this);
		}

		//Create inputcontroller
		if (!CInput::Initialize(EInputType::INPUT_TYPE_DEFAULT))
		{
			return false;
		}

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

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CEngine::RunMainLoop() 
	{
		//Startup engine
		m_State.bIsRunning = true;

        CConsole::PrintLine("Starting up engine");
        
		//MainLoop
		while (m_State.bIsRunning)
		{
			m_pSystem->ProcessSystemEvents();
			DoFrame();
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CEngine::DoFrame()
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool CEngine::OnSystemEvent(const SSystemEvent&)
	{
		return false;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CEngine::Exit(int32 exitCode)
	{
		m_State.bIsRunning	= false;
		m_State.ExitCode	= exitCode;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CEngine::Release()
	{
		//Remove as listener
		m_pSystem->RemoveEventListener(this);
		
		//Delete
		delete this;
	}
}
