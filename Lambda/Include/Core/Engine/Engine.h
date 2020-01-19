#pragma once
#include "LambdaCore.h"
#include "Singleton.h"
#include "LayerStack.h"

#include "Core/Time/Clock.h"

#include "Core/Event/ISystemEventListener.h"

namespace Lambda
{
    class CLayer;
    class CWindow;
    class CLogManager;
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
    
    class LAMBDA_API CEngine final : public CSingleton<CEngine>, 
        public ISystemEventListener
    {
    private:
        CEngine();
    public:
        ~CEngine() = default;

        bool Init(const SEngineParams& engineParams);
        void Startup();
        void Tick();
        void Terminate(int32 exitCode);
        void Release();
        
        bool IsRunning() const { return m_bIsRunning; }    
        
        void SetTimestep(const CTime& timestep) { m_Timestep = timestep; }
        
        int32 GetExitCode() const { return m_ExitCode; }
        const CTime& GetTimestep() const { return m_Timestep; }
        CLayerStack* GetLayerStack() { return m_pLayerStack; }
        
        static CEngine* Create();

        /*ISystemEventListener Interface*/
        virtual bool OnSystemEvent(const SSystemEvent& event) override final;
    private:
        CWindow* m_pWindow;
        CApplication* m_pApplication;
        CLayerStack* m_pLayerStack;
        CLogManager* m_pLogManager;

        CClock m_FrameClock;
        CTime m_Timestep;
        CTime m_UpdateBacklog;
        int32 m_ExitCode;
        bool  m_bIsRunning;
    };
}
