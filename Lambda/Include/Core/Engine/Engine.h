#pragma once
#include "LambdaCore.h"

#include "Core/Time/CClock.h"

#include "Core/Event/ISystemEventListener.h"

namespace Lambda
{
    //-------------
    //SEngineParams
    //-------------
    struct SEngineParams
    {
        const char** ppCmdLineArgs = nullptr;
        uint32 nCmdLineArgsCount = 0;
    };

    //-------
    //CEngine
    //-------
    class CEngine final : public ISystemEventListener
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
            int32 nExitCode;
        };
    public:
        /*ISystemEventListener Interface*/
        virtual bool OnSystemEvent(const SSystemEvent& event) override final;

        /*CEngine Interface*/
        bool Init(const SEngineParams& engineParams);
        
        void RunMainLoop();
        void Terminate(int32 nExitCode);

        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        void SetUpdateTimestep(const CTime& timestep)
        {
            m_Frametime.Timestep = timestep;
        }

        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        const CTime& GetUpdateTimestep() const
        { 
            return m_Frametime.Timestep; 
        }
    private:
        CEngine();
        ~CEngine();

        void DoFrame();
    private:
        SFrametime   m_Frametime;
        SEngineState m_State;
    public:
        static bool Initialize(const SEngineParams& engineParams);
        static void Release();
        static CEngine& Get();
    private:
        static CEngine* s_pInstance;
    };
}
