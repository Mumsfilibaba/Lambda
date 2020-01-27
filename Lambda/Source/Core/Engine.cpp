#include "LambdaPch.h"

#include "Core/Game.h"
#include "Core/Engine.h"
#include "Core/EngineLoop.h"

#include "Core/Event/Event.h"
#include "Core/Log/EngineLog.h"

#include "Platform/Platform.h"

namespace Lambda
{
    LAMBDA_API CEngine g_Engine;
    LAMBDA_API CreateGameFunc _CreateGame = nullptr;

    CEngine::CEngine()
        : m_pGame(nullptr),
        m_pWindow(nullptr)
    {
    }

    void CEngine::Init()
    {
        //Create window
        SWindowDesc desc = {};
        desc.pTitle = "Lambda Engine";
        desc.Width  = 1440;
        desc.Height = 900;

        m_pWindow = Platform::CreateWindow(desc);
        m_pWindow->AddEventListener(this);

        //Create game instance
        LAMBDA_ASSERT_PRINT(_CreateGame, "CreateGame must be set");
        
        m_pGame = _CreateGame();
        m_pGame->Init();

        LOG_INFO("Created Game Instance");
    }

    void CEngine::Update(const CTimestep& deltaTime)
    {
        m_pGame->Update(deltaTime);
    }

    void CEngine::FixedUpdate(const CTimestep& deltaTime)
    {
        m_pGame->FixedUpdate(deltaTime);
    }

    void CEngine::Render(const CTimestep& deltaTime)
    {
        m_pGame->Render(deltaTime);
    }

    void CEngine::Release()
    {
        m_pWindow->RemoveEventListener(this);

        SafeDelete(m_pWindow);
        SafeDelete(m_pGame);
    }

	bool CEngine::OnEvent(const SEvent& event)
	{
        if (event.EventType == EEventType::EVENT_TYPE_WINDOW_CLOSED)
        {
            g_EngineLoop.Terminate();
            return true;
        }

		return false;
	}
}
