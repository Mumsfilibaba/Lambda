#pragma once
#include "CSingleton.h"
#include "CLayerStack.h"

#include "Time/CClock.h"

namespace Lambda
{
	class CTime;
	class CLayer;
	class CLogManager;
	class CApplication;

	//-------------
	//SEngineParams
	//-------------
	struct SEngineParams
	{
		const char** ppCmdArgs	= nullptr;
		uint32	CmdArgsCount	= 0;
	};

    //-----------------------------------------------
    //CreateGameLayer - NEEDS TO BE DEFINED BY CLIENT
    //-----------------------------------------------
    CLayer* CreateGameLayer();

	//----------
	//LambdaMain
	//----------
	int32 LAMBDA_API LambdaMain(const SEngineParams& params);

	//-------
	//CEngine
	//-------
	class LAMBDA_API CEngine final : public CSingleton<CEngine>
	{
	private:
		//----------
		//SFrameTime
		//----------
		struct SFrameTime
		{
			CTime Timestep;
			CTime Accumulator;
			CClock Clock;
			uint32 UPS;
		};
	public:
		CEngine();
		CEngine::~CEngine();

		void RunMainLoop();
		void Release();
		void Exit(int32 exitCode = 0);
		
		//Deltatime between logic-updates
		_forceinline void SetUpdateTimeStep(const CTime& timestep)
		{
			LAMBDA_ASSERT_PRINT(timestep.AsNanoSeconds() > 0, "Timestep must be more than zero\n");
			m_FrameTime.Timestep = timestep;
		}

		_forceinline const CTime& GetUpdateTimeStep() const
		{
			return m_FrameTime.Timestep;
		}

		_forceinline int32 GetExitCode() const
		{
			return m_ExitCode;
		}
	private:
		bool InternalInitialize(const SEngineParams& params);
		void DoFrame();
	public:
		CApplication* m_pApplication;
		CLogManager* m_pLogManager;
		CEventDispatcher* m_pEventDispatcher;
		CLayerStack m_LayerStack;
		SFrameTime m_FrameTime;
		int32 m_ExitCode;
		bool m_bIsRunning;
	public:
		static bool Initialize(const SEngineParams& params);
	};
}
