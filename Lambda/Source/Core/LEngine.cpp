#include "LambdaPch.h"
#include "Core/LEngine.h"
#include "Core/Host.h"
#include "Core/LogManager.h"
#include "Core/WindowEventDispatcher.h"

namespace Lambda
{
	//----------
	//LambdaMain
	//----------

	int32 LambdaMain(const LEngineParams& params)
	{
		DBG_MEMLEAK_CHECK();

		LEngine* pLEngine = DBG_NEW LEngine();
		pLEngine->Init(params);
		pLEngine->Run();
		pLEngine->Release();
		return 0;
	}

	//-------
	//LEngine
	//-------

	LEngine::LEngine()
		: Singleton<LEngine>(),
		IHostEventListener(),
		m_pHost(nullptr),
		m_pLogManager(nullptr),
		m_IsRunning(true)
	{
	}


	LEngine::~LEngine()
	{
		m_pWindowEventDispatcher->Release();
		m_pLogManager->Release();
		m_pHost->Release();
	}


	void LEngine::Release()
	{
		delete this;
	}


	void LEngine::Init(const LEngineParams& params)
	{
		//Create Host
		m_pHost = Host::Create();

		//Init LogManager
		m_pLogManager = DBG_NEW LogManager();
		//Init WindowEventDispatcher
		m_pWindowEventDispatcher = DBG_NEW WindowEventDispatcher();
				
		//Init Host
		m_pHost->Init();
		m_pHost->AddEventListener(this);
	}


	void LEngine::Run()
	{
		while (m_IsRunning)
		{
			m_pHost->ProcessEvents();
			DoFrame();
		}
	}

	
	void LEngine::DoFrame()
	{
	}
	
	
	void LEngine::OnHostQuit(int32 exitCode)
	{
		LOG_ENGINE_INFO("Quiting with code %d\n", exitCode);

		m_ExitCode	= exitCode;
		m_IsRunning = false;
	}
}