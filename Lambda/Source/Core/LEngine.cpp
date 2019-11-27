#include "LambdaPch.h"
#include "Core/LEngine.h"
#include "Core/Environment.h"
#include "Core/LayerStack.h"
#include "Core/LogManager.h"
#include "Core/WindowEventDispatcher.h"
#include "Core/Layer.h"
#include "Time/Clock.h"

//--------------------------------------------------------------------------------------------------------
//_CreateGameLayer - Function pointer to create the gamelayer - Needed for DLL- and Static-Lib compilation
//--------------------------------------------------------------------------------------------------------

LAMBDA_API Lambda::Layer* (*_CreateGameLayer)(void) = nullptr;

namespace Lambda
{
	//----------
	//LambdaMain
	//----------

	template<>
	LEngine* Singleton<LEngine>::s_pInstance = nullptr;

	int32 LambdaMain(const LEngineParams& params)
	{
		DBG_MEMLEAK_CHECK();

		LEngine* pLEngine = DBG_NEW LEngine();
		pLEngine->Initialize(params);
		pLEngine->Run();
		pLEngine->Release();
		return 0;
	}

	//-------
	//LEngine
	//-------

	LEngine::LEngine()
		: Singleton<LEngine>(),
		IEnvironmentEventListener(),
		m_pEnvironment(nullptr),
		m_pLogManager(nullptr),
		m_pWindowEventDispatcher(nullptr),
		m_LayerStack(),
		m_FrameClock(),
		m_FrameAccumulator(),
		m_Timestep(Timestep::Seconds(1.0f / 60.0f)),
		m_IsRunning(true)
	{
	}


	LEngine::~LEngine()
	{
		//Release layers
		m_LayerStack.ReleaseLayers();

		//Release subsystems
		m_pWindowEventDispatcher->Release();
		m_pLogManager->Release();
		m_pEnvironment->Release();
	}


	void LEngine::Release()
	{
		delete this;
	}


	void LEngine::Initialize(const LEngineParams& params)
	{
		//Create Host
		m_pEnvironment = Environment::Create();

		//Init LogManager
		m_pLogManager = DBG_NEW LogManager();
		//Init WindowEventDispatcher
		m_pWindowEventDispatcher = DBG_NEW WindowEventDispatcher();
				
		//Init Host
		m_pEnvironment->Init();
		m_pEnvironment->AddEventListener(this);

		//Create Game Layer
		if (_CreateGameLayer != nullptr)
		{
			m_LayerStack->PushLayer(_CreateGameLayer());
		}
		else
		{
			//TODO: RETURN FATAL ERROR
		}
	}


	void LEngine::Run()
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

	
	void LEngine::DoFrame()
	{
		static uint32 ups = 0;

		//Logic update
		m_FrameClock.Tick();
		m_FrameAccumulator += m_FrameClock.GetDeltaTime();
		while (m_FrameAccumulator >= m_Timestep)
		{
			//Update all layers
			for (Layer* pLayer : m_LayerStack)
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
	
	
	void LEngine::OnHostQuit(int32 exitCode)
	{
		LOG_ENGINE_INFO("Quiting with code %d\n", exitCode);

		m_ExitCode	= exitCode;
		m_IsRunning = false;
	}
}