#pragma once
#include "LambdaCore.h"
#include "IHostEventListener.h"
#include "Utilities/Singleton.h"

namespace Lambda
{
	class Host;
	class Layer;
	class LayerStack;
	class LogManager;
	class WindowEventDispatcher;

	//------------
	//EngineParams
	//------------

	struct LEngineParams
	{
		const char** ppCmdArgs	= nullptr;
		uint32	CmdArgsCount	= 0;
		Layer** ppLayers		= nullptr;
		uint32	LayerCount		= 0;
	};

	//----------
	//LambdaMain
	//----------

	int32 LAMBDA_API LambdaMain(const LEngineParams& params);

	//-------
	//LEngine
	//-------

	class LAMBDA_API LEngine final : public Singleton<LEngine>, public IHostEventListener
	{
	public:
		LEngine();
		~LEngine();

		void Init(const LEngineParams& params);
		void Run();
		void DoFrame();
		void Release();

		//SystemEventCallbacks
		virtual void OnHostQuit(int32 exitCode) override final;
	private:
		Host* m_pHost;
		LogManager* m_pLogManager;
		LayerStack* m_pLayerStack;
		WindowEventDispatcher* m_pWindowEventDispatcher;
		int32 m_ExitCode;
		bool m_IsRunning;
	};
}