#include "LambdaPch.h"

#include "Core/Game.h"
#include "Core/Engine.h"
#include "Core/IWindow.h"

#include "Core/Event/Event.h"
#include "Core/Log/EngineLog.h"

#include "Core/Debug/Profiler.h"
#include "Core/Debug/IConsoleOutput.h"

#include "Platform/Platform.h"
#include "Platform/PlatformInput.h"

namespace Lambda
{
    LAMBDA_API CEngine g_Engine;

    CEngine::CEngine()
        : m_Frameclock(),
        m_FrameAccumulator(0),
        m_DesiredFrameTime(CTimestamp::Seconds(1.0f / 60.0f)),
        m_pGame(nullptr),
        m_pWindow(nullptr),
        m_bIsRunning(false)
    {

    }

    void CEngine::PreInit()
    {
        //Init console
        m_pConsoleOutput = Platform::CreateConsoleOutput();
        if (m_pConsoleOutput)
        {
            m_pConsoleOutput->SetTitle("Lambda Engine Debug Console");
            m_pConsoleOutput->Show();
        }

        //Init log
        CEngineLog::Init();

        //Start profiling of the engine startup
        LAMBDA_PROFILER_BEGIN_SESSION("Lambda Init", "Lambda_Init.json");

        m_bIsRunning = false;
        
        LOG_CORE_INFO("Finished Pre-Init");
    }

    void CEngine::Init()
    {
        {
            LAMBDA_PROFILER_FUNCTION();

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
            LAMBDA_ASSERT_MSG(_CreateGameInstance, "CreateGame must be set");
        
            m_pGame = _CreateGameInstance();
            LOG_CORE_INFO("Created Game Instance");
            m_pGame->Init();

            LOG_CORE_INFO("Finished Init");
        }

        LAMBDA_PROFILER_END_SESSION();
    }

    void CEngine::Run()
    {
        LAMBDA_PROFILER_BEGIN_SESSION("Lambda Runtime", "Lambda_Runtime.json");

        {
            LAMBDA_PROFILER_FUNCTION();
            
            LOG_CORE_INFO("Starting Engine");

            //Start engine
            m_bIsRunning = true;

            //Make first tick on frameclock - To prevent that the first frame gets huge values in dt
            m_Frameclock.Tick();

            //Show window when engine is completly initialized
            m_pWindow->Show();

            while (IsRunning())
            {
                Tick();
            }

            LOG_CORE_INFO("Exiting Engine");
        }

        LAMBDA_PROFILER_END_SESSION();
    }

    void CEngine::Tick()
    {
        LAMBDA_PROFILER_FUNCTION();

        //Get deltatime
        m_Frameclock.Tick();
        CTimestamp deltatime = m_Frameclock.GetDeltaTime();

        //Poll platform events
        Platform::PollEvents();
        PlatformInput::Update();
        
        //Fixed update
        {
            LAMBDA_PROFILER_SCOPE("Fixed update");

            m_FrameAccumulator += deltatime;
            while (m_FrameAccumulator >= m_DesiredFrameTime)
            {
                m_pGame->FixedUpdate(m_DesiredFrameTime);
                if (deltatime > m_DesiredFrameTime)
                {
                    m_pGame->Update(m_DesiredFrameTime);
                    deltatime -= m_DesiredFrameTime;
                }

                m_FrameAccumulator -= m_DesiredFrameTime;
            }
        }

        //Perform the variable update
        {
            LAMBDA_PROFILER_SCOPE("Variable update");
            m_pGame->Update(deltatime);
        }

        {
            LAMBDA_PROFILER_SCOPE("Render");
            m_pGame->Render(deltatime);
        }
    }

    void CEngine::RequestExit()
    {
        LAMBDA_PROFILER_FUNCTION();

        LOG_CORE_INFO("RequestExit");
        m_bIsRunning = false;
    }

    void CEngine::Release()
    {
        LAMBDA_PROFILER_BEGIN_SESSION("Lambda Release", "Lambda_Release.json");

        LAMBDA_PROFILER_FUNCTION();

        LOG_CORE_INFO("Engine Release");
        
        m_pWindow->RemoveEventListener(this);

        SafeDelete(m_pGame);
        SafeDelete(m_pWindow);
    }

    void CEngine::PostRelease()
    {
        LOG_CORE_INFO("Engine Post-Release");

        LAMBDA_PROFILER_END_SESSION();

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
