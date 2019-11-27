#pragma once
#include "LambdaCore.h"
#include "IEnvironmentEventListener.h"
#include "Time/Clock.h"
#include "Time/Timestep.h"
#include "Utilities/Singleton.h"

namespace Lambda
{
	class Layer;
	class LayerStack;
	class LogManager;
	class Environment;
	class WindowEventDispatcher;

	//------------
	//EngineParams
	//------------

	struct LEngineParams
	{
		const char** ppCmdArgs	= nullptr;
		uint32	CmdArgsCount	= 0;
	};

	//----------
	//LambdaMain
	//----------

	int32 LAMBDA_API LambdaMain(const LEngineParams& params);
	
	//-----------------------------------------------
	//CreateGameLayer - NEEDS TO BE DEFINED BY CLIENT
	//-----------------------------------------------
	Layer* CreateGameLayer();

	//-------
	//LEngine
	//-------

	class LAMBDA_API LEngine final : public Singleton<LEngine>, public IEnvironmentEventListener
	{
	public:
		LEngine();
		~LEngine();

		void Initialize(const LEngineParams& params);
		void Run();
		void DoFrame();
		void Release();

		//SystemEventCallbacks
		virtual void OnHostQuit(int32 exitCode) override final;
	private:
		Environment* m_pEnvironment;
		LogManager* m_pLogManager;
		WindowEventDispatcher* m_pWindowEventDispatcher;
		LayerStack m_LayerStack;
		Clock m_FrameClock;
		Timestep m_FrameAccumulator;
		const Timestep m_Timestep;
		int32 m_ExitCode;
		bool m_IsRunning;
	};
}