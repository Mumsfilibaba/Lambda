#pragma once
#include "LambdaCore.h"

#include "Core/Time/CClock.h"
#include "Core/Event/SystemEventDispatcher.h"

namespace Lambda
{
    class ISystem;

    class CLayer;

    //-------------
    //SEngineParams
    //-------------
    struct SEngineParams
    {
        const char** ppCmdLineArgs = nullptr;
        uint32 nCmdLineArgsCount = 0;
    };

    //-------
    //IEngine
    //-------
    class LAMBDA_API IEngine
    {
    public:
        LAMBDA_INTERFACE(IEngine);

        virtual void Release() = 0;
        virtual void RunMainLoop() = 0;
        virtual void Exit(int32 exitCode) = 0;

        virtual void SetUpdateTimestep(const CTime& timestep) = 0;

        virtual ISystem*     GetSystem()         const = 0;
        virtual const CTime& GetUpdateTimestep() const = 0;
    private:
        virtual bool Initialize(const SEngineParams& engineParams) = 0;
    public:
        static bool Create(const SEngineParams& engineParams);
        static IEngine* Get();
    private:
        static IEngine* s_pInstance;
    };


    //-------
    //CEngine
    //-------
    class CEngine final : public IEngine, public ISystemEventListener
    {
    private:
        struct SFrametime
        {
            CClock FrameClock;
            CTime Timestep;
            CTime UpdateBacklog;
        };

        struct SEngineState
        {
            bool bIsRunning;
            int32 ExitCode;
        };
    public:
        CEngine();
        ~CEngine();

        /*ISystemEventListener Interface*/
        virtual bool OnSystemEvent(const SSystemEvent& event) override final;

        /*IEngine Interface*/
        virtual void Release() override final;
        virtual void RunMainLoop() override final;
        virtual void Exit(int32 exitCode) override final;

        virtual void SetUpdateTimestep(const CTime& timestep) override final
        {
            m_Frametime.Timestep = timestep;
        }

        virtual ISystem*     GetSystem() const override final         { return m_pSystem; }
        virtual const CTime& GetUpdateTimestep() const override final { return m_Frametime.Timestep; }
    private:
        virtual bool Initialize(const SEngineParams& engineParams) override final;
      
        void DoFrame();
    private:
        ISystem* m_pSystem;

        SFrametime m_Frametime;
        SEngineState m_State;
    };
}
