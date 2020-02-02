#include "LambdaPch.h"

#include "Core/Game.h"
#include "Core/Engine.h"

#include "Core/Event/Event.h"
#include "Core/Log/EngineLog.h"
#include "Core/Debug/IConsoleOutput.h"

#include "Platform/Platform.h"
#include "Platform/Common/IWindow.h"

namespace Lambda
{
    LAMBDA_API CEngine g_Engine;

    CEngine::CEngine()
        : m_Frameclock(),
        m_pGame(nullptr),
        m_pWindow(nullptr),
        m_bIsRunning(false)
    {
    }

    void CEngine::PreInit()
    {
        m_bIsRunning = false;

        //Init console
        m_pConsoleOutput = Platform::CreateConsoleOutput();
        if (m_pConsoleOutput)
        {
            m_pConsoleOutput->SetTitle("Lambda Engine Debug Console");
            m_pConsoleOutput->Show();
        }

        //Init log
        CEngineLog::Init();

        LOG_CORE_INFO("Finished Pre-Init");
    }

    void CEngine::Init(CreateGameInstanceFunc pfnCreateGameInstanceFunc)
    {
        //Create window
        SWindowDesc desc = {};
        desc.pTitle = "Lambda Engine";
        desc.Width  = 1440;
        desc.Height = 900;

        m_pWindow = Platform::CreateWindow(desc);
        m_pWindow->AddEventListener(this);

        //Create game instance
        LAMBDA_ASSERT_PRINT(pfnCreateGameInstanceFunc, "CreateGame must be set");
        
        m_pGame = pfnCreateGameInstanceFunc();
        m_pGame->Init();

        LOG_INFO("Created Game Instance");

        LOG_CORE_INFO("Finished Init");
    }

    void CEngine::Start()
    {
        LOG_CORE_INFO("Starting Engine");

        //Start engine
        m_bIsRunning = true;

        //Make first tick on frameclock - To prevent that the first frame gets huge values in dt
        m_Frameclock.Tick();

        //Show window when engine is completly initialized
        g_Engine.GetWindow().Show();
    }

    void CEngine::Tick()
    {
        Platform::PollEvents();
        m_Frameclock.Tick();

        //Input::Update();

        //Perform the fixed update
        CTimestep deltatime = m_Frameclock.GetDeltaTime();
        m_pGame->FixedUpdate(deltatime);

        //Perform the variable update
        m_pGame->Update(deltatime);
        m_pGame->Render(deltatime);
    }

    void CEngine::RequestExit()
    {
        LOG_CORE_INFO("Terminating Engine");
        m_bIsRunning = false;
    }

    void CEngine::Release()
    {
        LOG_CORE_INFO("Engine Release");
        
        m_pWindow->RemoveEventListener(this);

        SafeDelete(m_pGame);
        SafeDelete(m_pWindow);
    }

    void CEngine::PostRelease()
    {
        LOG_CORE_INFO("Engine Post-Release");

        CEngineLog::Release();

        //Delete output after log is released since the log can still write to the console
        SafeDelete(m_pConsoleOutput);
    }

    bool CEngine::OnEvent(const SEvent& event)
    {
        if (event.EventType == EEventType::EVENT_TYPE_WINDOW_CLOSED)
        {
            RequestExit();
            return true;
        }

        return false;
    }
}
