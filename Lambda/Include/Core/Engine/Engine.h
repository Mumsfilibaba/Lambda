#pragma once
#include "LambdaCore.h"

#include "LayerStack.h"

#include "Core/Time/Clock.h"
#include "Core/Event/ISystemEventListener.h"

namespace Lambda
{
    class ILog;
    class IWindow;
    class Layer;
    class LogManager;

    //------------
    //EngineParams
    //------------

    struct EngineParams
    {
        const char** ppCmdLineArgs = nullptr;
        uint32 CmdLineArgsCount = 0;
    };

    //------
    //Engine
    //------
    
    class LAMBDA_API Engine final : public ISystemEventListener
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
        Engine();
        ~Engine();

        /*ISystemEventListener Interface*/
        virtual bool OnSystemEvent(const SystemEvent& event) override final;

        /*Engine Interface*/
        bool Init(const EngineParams& engineParams);
        void Detach();

        void RunMainLoop();
        void Exit(int32 exitCode);

        bool IsRunning() const { return m_State.IsRunning; }

        void PushLayer(Layer* pLayer) { m_pLayerStack->PushLayer(pLayer); }
        void PopLayer() { m_pLayerStack->PopLayer(); }

        void SetUpdateTimestep(const Time& timestep) { m_Frametime.Timestep = timestep; }
        
        int32 GetExitCode() const { return m_State.ExitCode; }
        const Time& GetUpdateTimestep() const { return m_Frametime.Timestep; }
    private:
        IWindow* m_pWindow;
        LayerStack* m_pLayerStack;
        LogManager* m_pLogManager;

        Frametime   m_Frametime;
        EngineState m_State;
    public:
        static Engine& Get();
    };
}
