#pragma once
#include "LambdaCore.h"

#include "Core/Time/CClock.h"

namespace Lambda
{
    class CLayer;

    class IInput;
    class ISystem;

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

        virtual void RunMainLoop() = 0;
        virtual void Exit(int32 exitCode) = 0;
        virtual void Release() = 0;

        virtual void SetUpdateTimestep(const CTime& timestep) = 0;

        virtual IInput* GetInput() const = 0;
        virtual ISystem* GetSystem() const = 0;

        virtual const CTime& GetUpdateTimestep() const = 0;
    private:
        virtual bool InternalInit(const SEngineParams& engineParams) = 0;
    public:
        static bool Initialize(const SEngineParams& engineParams);
        static IEngine* Get();
    private:
        static IEngine* s_pEngineInstance;
    };


    //-------
    //CEngine
    //-------
    class CEngine final : public IEngine
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

        virtual void RunMainLoop() override final;
        virtual void Exit(int32 exitCode) override final;
        virtual void Release() override final;

        virtual void SetUpdateTimestep(const CTime& timestep) override final
        {
            m_Frametime.Timestep = timestep;
        }

        virtual IInput*   GetInput()    const override final { return m_pInput; }
        virtual ISystem*  GetSystem()   const override final { return m_pSystem; }
        
        virtual const CTime& GetUpdateTimestep() const override final { return m_Frametime.Timestep; };
    private:
        virtual bool InternalInit(const SEngineParams& engineParams) override final;
    private:
        ISystem* m_pSystem;
        IInput* m_pInput;

        SFrametime m_Frametime;
        SEngineState m_State;
    };
}
