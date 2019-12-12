#include "LambdaPch.h"
#include "Core/CEngine.h"
#include "Core/CEnvironment.h"
#include "Core/CLayerStack.h"
#include "Core/CLogManager.h"
#include "Core/Input/CKeyboard.h"
#include "Core/Input/CMouse.h"
#include "Core/Input/CGamepad.h"
#include "Core/CLayer.h"
#include "Core/Event/CQuitEvent.h"
#include "Time/CClock.h"

//--------------------------------------------------------------------------------------------------------
//_CreateGameLayer - Function pointer to create the gamelayer - Needed for DLL- and Static-Lib compilation
//--------------------------------------------------------------------------------------------------------

LAMBDA_API Lambda::CLayer* (*_CreateGameLayer)(void) = nullptr;

namespace Lambda
{
	//----------
	//LambdaMain
	//----------

	template<>
	CEngine* CSingleton<CEngine>::s_pInstance = nullptr;

	int32 LambdaMain(const SEngineParams& params)
	{
		DBG_MEMLEAK_CHECK();

		CEngine* pLEngine = DBG_NEW CEngine();
		pLEngine->Initialize(params);
		pLEngine->Run();
		pLEngine->Release();
		return 0;
	}

	//-------
	//CEngine
	//-------

	CEngine::CEngine()
		: CSingleton<CEngine>(),
		IEventListener(),
		m_pEnvironment(nullptr),
		m_pLogManager(nullptr),
		m_LayerStack(),
		m_FrameClock(),
		m_FrameAccumulator(),
		m_Timestep(CTime::Seconds(1.0f / 60.0f)),
		m_IsRunning(true)
	{
	}


	void CEngine::Release()
	{
		//Release layers
		m_LayerStack.ReleaseLayers();

		//Release subsystems
		m_pLogManager->Release();
		m_pEnvironment->Release();

		delete this;
	}

	
	void CEngine::Initialize(const SEngineParams&)
	{
		//Create Host
		m_pEnvironment = CEnvironment::Create();

		//Init LogManager
		m_pLogManager = DBG_NEW CLogManager();
				
		//Init Host
		m_pEnvironment->Initialize();
		m_pEnvironment->AddEventListener(this);

		//Create Game Layer
		if (_CreateGameLayer != nullptr)
		{
            m_LayerStack.PushLayer(_CreateGameLayer());
		}
		else
		{
			//TODO: RETURN FATAL ERROR
		}
	}


	void CEngine::Run()
	{
		LOG_ENGINE_INFO("STARTING\n");

		//Reset clock before starting the loop or we get inaccurate timings
		m_FrameClock.Reset();

		//Start loop
		while (m_IsRunning)
		{
			m_pEnvironment->ProcessEvents();         
			DoFrame();
		}
	}

	
	void CEngine::DoFrame()
	{
		static uint32 ups = 0;

		//Logic update
		m_FrameClock.Tick();
		m_FrameAccumulator += m_FrameClock.GetDeltaTime();
		while (m_FrameAccumulator >= m_Timestep)
		{
			//Update input at the same rate as update
			CMouse::Update();
			CGamepad::Update();
			CKeyboard::Update();

			//Update all layers
			for (auto pLayer : m_LayerStack)
				pLayer->OnUpdate(m_Timestep);

			m_FrameAccumulator -= m_Timestep;
			ups++;
		}

		//Print UPS to console
		if (m_FrameClock.GetTotalTime().AsSeconds() >= 1.0f)
		{
			LOG(LOG_CHANNEL_ENGINE, LOG_SEVERITY_INFO, "UPS: %u, Frametime: %.4fms\n", ups, m_FrameClock.GetDeltaTime().AsMilliSeconds());
			ups = 0;
			m_FrameClock.Reset();
		}
	}
	
	
	bool CEngine::OnEvent(const CEvent& event)
	{
        //Get exit code and exit the run loop
        if (event.GetType() == CQuitEvent::GetStaticType())
        {
            const CQuitEvent& quit = static_cast<const CQuitEvent&>(event);
            
			LOG_ENGINE_INFO("Quiting with code %d\n", quit.GetExitCode());
            
            m_ExitCode  = quit.GetExitCode();
            m_IsRunning = false;
            return true;
        }
        
        return false;
	}
}
