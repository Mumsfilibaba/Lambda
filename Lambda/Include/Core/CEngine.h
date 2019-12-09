#pragma once
#include "LambdaCore.h"
#include "Event/IEventListener.h"
#include "CSingleton.h"
#include "Time/Clock.h"
#include "Time/Timestep.h"

namespace Lambda
{
	class Layer;
	class LayerStack;
	class LogManager;
	class CEnvironment;

	//-------------
	//SEngineParams
	//-------------

	struct SEngineParams
	{
		const char** ppCmdArgs	= nullptr;
		uint32	CmdArgsCount	= 0;
	};

	//----------
	//LambdaMain
	//----------

	int32 LAMBDA_API LambdaMain(const SEngineParams& params);
	
	//-----------------------------------------------
	//CreateGameLayer - NEEDS TO BE DEFINED BY CLIENT
	//-----------------------------------------------
	Layer* CreateGameLayer();

	//-------
	//CEngine
	//-------

	class LAMBDA_API CEngine final : public CSingleton<CEngine>, public IEventListener
	{
	public:
		CEngine();
		~CEngine();

        LAMBDA_NO_COPY(CEngine);
        
        virtual void OnEvent(const CEvent& event) override final;

        void Initialize(const SEngineParams& params);
        void Run();
        void DoFrame();
        void Release();
	private:
		CEnvironment* m_pEnvironment;
		LogManager* m_pLogManager;
		LayerStack m_LayerStack;
		Clock m_FrameClock;
		Timestep m_FrameAccumulator;
		const Timestep m_Timestep;
		int32 m_ExitCode;
		bool m_IsRunning;
	};
}
