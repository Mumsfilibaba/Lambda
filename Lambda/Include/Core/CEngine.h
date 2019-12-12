#pragma once
#include "Event/IEventListener.h"
#include "CSingleton.h"
#include "CLayerStack.h"
#include "Time/CClock.h"

namespace Lambda
{
	class CLayer;
	class CLayerStack;
	class CLogManager;
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
	CLayer* CreateGameLayer();

	//-------
	//CEngine
	//-------

	class LAMBDA_API CEngine final : public CSingleton<CEngine>, public IEventListener
	{
	public:
		CEngine();
		~CEngine() = default;

        LAMBDA_NO_COPY(CEngine);
        
        virtual bool OnEvent(const CEvent& event) override final;

        void Initialize(const SEngineParams& params);
        void Run();
        void Release();
	private:
        void DoFrame();
	private:
		CEnvironment* m_pEnvironment;
		CLogManager* m_pLogManager;
		CLayerStack m_LayerStack;
		CClock m_FrameClock;
		CTime m_FrameAccumulator;
		const CTime m_Timestep;
		int32 m_ExitCode;
		bool m_IsRunning;
	};
}
