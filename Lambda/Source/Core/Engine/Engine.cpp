#include "LambdaPch.h"

#include "Core//Platform.h"

#include "Core/Engine/Engine.h"
#include "Core/Engine/Console.h"
#include "Core/Engine/Application.h"

#include "Core/Input/Input.h"

//----------------
//_CreateGameLayer
//----------------
namespace Lambda
{
	class CLayer;
}

extern Lambda::CLayer* (*_CreateGameLayer)();

namespace Lambda
{
	//-------
	//CEngine
	//-------

	CEngine* CEngine::s_pInstance = nullptr;
	
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CEngine& CEngine::Get()
	{
		LAMBDA_ASSERT_PRINT(s_pInstance != nullptr, "Engine has not been initialized");
		return *s_pInstance;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool CEngine::Initialize(const SEngineParams& engineParams)
	{
		LAMBDA_ASSERT_PRINT(s_pInstance == nullptr, "Engine can only be created once");

		s_pInstance = DBG_NEW CEngine();
		return s_pInstance->Init(engineParams);
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CEngine::Release()
	{
		//Delete
		SafeDelete(s_pInstance);
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CEngine::CEngine()
		: ISystemEventListener()
	{
		//Setup default frametime
		m_Frametime.Timestep	  = CTime::Seconds(1.0f / 60.0f);
		m_Frametime.UpdateBacklog = CTime(0);

		//Setup enginestate
		m_State.bIsRunning = false;
		m_State.nExitCode = 0;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CEngine::~CEngine()
	{
		//Remove as listener
		CApplication::RemoveListener(this);

		//Release subsystems
		CInput::Release();

#if defined(LAMBDA_DEVELOP)
		//Release console
		CConsole::Release();
#endif
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool CEngine::Init(const SEngineParams&)
	{
		//Create application
		CApplication::Initialize();
#if defined(LAMBDA_DEVELOP)
		//Create console
		CConsole::Initialize();
#endif
  
		//Create window
		if (!CApplication::CreateWindow("Lambda Engine", 1440, 900))
		{
			return false;
		}

		//Add this to the system event listeners
		CApplication::AddListener(this);

		//Create inputcontroller
		CInput::Initialize(EInputType::INPUT_TYPE_DEFAULT);

		//Create gamelayer
		if (_CreateGameLayer)
		{
			_CreateGameLayer();
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
			CApplication::ProcessEvents();
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
	void CEngine::Terminate(int32 nExitCode)
	{
		m_State.bIsRunning = false;
		m_State.nExitCode  = nExitCode;
	}
}
