#include "LambdaPch.h"

#include "Core/Game.h"
#include "Core/Engine.h"
#include "Core/IWindow.h"

#include "Core/Event/Event.h"
#include "Core/Log/EngineLog.h"
#include "Core/Debug/IConsoleOutput.h"

#include "Platform/Platform.h"
#include "Platform/PlatformInput.h"

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

    void CEngine::Init()
    {
        //Create window
        SWindowDesc windowDesc = {};
        windowDesc.pTitle = "Lambda Engine";
        windowDesc.Width  = 1440;
        windowDesc.Height = 900;
        
        m_pWindow = Platform::CreateWindow(windowDesc);
        if (m_pWindow)
        {
            m_pWindow->AddEventListener(this);
        }

        //Init input
        PlatformInput::Init();

        //Create game instance
        LAMBDA_ASSERT_PRINT(_CreateGameInstance, "CreateGame must be set");
        
        m_pGame = _CreateGameInstance();
        LOG_CORE_INFO("Created Game Instance");
        m_pGame->Init();

        LOG_CORE_INFO("Finished Init");
    }

    void CEngine::Run()
    {
        LOG_CORE_INFO("Starting Engine");

        //Start engine
        m_bIsRunning = true;

        //Make first tick on frameclock - To prevent that the first frame gets huge values in dt
        m_Frameclock.Tick();

        //Show window when engine is completly initialized
        m_pWindow->Show();

        //Run engine loop
        while (IsRunning())
        {
            Tick();
        }

        LOG_CORE_INFO("Exiting Engine");
    }

    void CEngine::Tick()
    {
        Platform::PollEvents();
        PlatformInput::Update();

        //Get deltatime
        m_Frameclock.Tick();
        CTimestep deltatime = m_Frameclock.GetDeltaTime();
        
        //TODO: Perform the fixed update
        m_pGame->FixedUpdate(deltatime);

        //Perform the variable update
        m_pGame->Update(deltatime);
        m_pGame->Render(deltatime);
    }

    void CEngine::RequestExit()
    {
        LOG_CORE_INFO("RequestExit");
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
        else if (event.EventType == EEventType::EVENT_TYPE_WINDOW_RESIZE)
        {
            D_LOG_CORE_INFO("Window Resize w=%d, h=%d", event.WindowResizeEvent.Width, event.WindowResizeEvent.Height);
        }

        return false;
    }
}
