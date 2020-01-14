#pragma once
#include "LambdaCore.h"
#include "Singleton.h"
#include "LayerStack.h"

#include "Core/Time/Clock.h"

#include "Core/Event/ISystemEventListener.h"

namespace Lambda
{
    class ILog;
    class Layer;
    class CLogManager;
    class CWindow;
    class CApplication;

    //-------------
    //SEngineParams
    //-------------

    struct SEngineParams
    {
        const char** ppCmdLineArgs = nullptr;
        uint32 CmdLineArgsCount = 0;
    };

    //-------
    //CEngine
    //-------
    
    class LAMBDA_API CEngine final :
        public CSingleton<CEngine>,
        public ISystemEventListener
    {
    private:
        struct Frametime
        {
            Clock FrameClock;
            Time Timestep;
            Time UpdateBacklog;
        };

        struct EngineState
        {
            bool IsRunning;
            int32 ExitCode;
        };
    public:
        CEngine();
        ~CEngine();

        /*ISystemEventListener Interface*/
        virtual bool OnSystemEvent(const SystemEvent& event) override final;

        /*Engine Interface*/
        bool Init(const SEngineParams& engineParams);
        void Release();

        void Tick();
        void Exit(int32 exitCode);

        bool IsRunning() const { return m_State.IsRunning; }

        void PushLayer(Layer* pLayer) { m_pLayerStack->PushLayer(pLayer); }
        void PopLayer() { m_pLayerStack->PopLayer(); }

        void SetUpdateTimestep(const Time& timestep) { m_Frametime.Timestep = timestep; }
        
        int32 GetExitCode() const { return m_State.ExitCode; }
        const Time& GetUpdateTimestep() const { return m_Frametime.Timestep; }
    private:
        CWindow* m_pWindow;
        CApplication* m_pApplication;
        LayerStack* m_pLayerStack;
        CLogManager* m_pLogManager;

        Frametime   m_Frametime;
        EngineState m_State;
    };
}
