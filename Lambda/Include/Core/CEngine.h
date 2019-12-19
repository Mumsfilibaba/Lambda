#pragma once
#include "CSingleton.h"
#include "CLayerStack.h"
#include "CEnvironment.h"

#include "Event/IEventListener.h"
#include "Event/CEventDispatcher.h"

#include "Time/CClock.h"

namespace Lambda
{
	class CLayer;
	class CEngine;
	class CLayerStack;
	class CLogManager;
	class CEnvironment;
	class IMouseController;
	class IGamepadController;
	class IKeyboardController;

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

	//---------------------
	//CreateEngineInterface
	//---------------------
	CEngine* CreateEngineInterface(const SEngineParams& params);
	
	//-----------------------------------------------
	//CreateGameLayer - NEEDS TO BE DEFINED BY CLIENT
	//-----------------------------------------------
	CLayer* CreateGameLayer();

	//-------
	//CEngine
	//-------

	class LAMBDA_API CEngine final : public CSingleton<CEngine>
	{
	public:
		CEngine();
		~CEngine() = default;

        LAMBDA_NO_COPY(CEngine);

        bool Initialize(const SEngineParams& params);
        void RunMainLoop();
        void Release();

		void SetMouseController(IMouseController* pMouseController);
		void SetKeyboardController(IKeyboardController* pKeyboardController);
		void SetGamepadController(IGamepadController* pGamepadController);

		_forceinline int32 GetExitCode() const
		{
			return m_ExitCode;
		}

		_forceinline IKeyboardController* GetKeyboardController() const
		{
			LAMBDA_ASSERT_PRINT(m_pKeyboardController != nullptr, "Environment has not created a KeyboardController\n");
			return m_pKeyboardController;
		}

		_forceinline IMouseController* GetMouseController() const
		{
			LAMBDA_ASSERT_PRINT(m_pMouseController != nullptr, "Environment has not created a MouseController\n");
			return m_pMouseController;
		}

		_forceinline IGamepadController* GetGamepadController() const
		{
			LAMBDA_ASSERT_PRINT(m_pGamepadController != nullptr, "Environment has not created a GamepadController\n");
			return m_pGamepadController;
		}
	private:
        void DoFrame();
		void Exit(int32 exitCode);
	private:
		CEnvironment* m_pEnvironment;
		//--------
		//Managers
		//--------
		CLogManager*	  m_pLogManager;
		CEventDispatcher* m_pEventDispatcher;
		//-----------------
		//Input controllers
		//-----------------
		IMouseController*	 m_pMouseController;
		IGamepadController*  m_pGamepadController;
		IKeyboardController* m_pKeyboardController;

		CLayerStack m_LayerStack;
		CClock m_FrameClock;
		CTime m_FrameAccumulator;
		const CTime m_Timestep;
		int32 m_ExitCode;
		bool m_IsRunning;
	};
}
