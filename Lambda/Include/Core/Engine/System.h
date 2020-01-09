#pragma once
#include "LambdaCore.h"

#ifdef CreateWindow
	#undef CreateWindow
#endif

namespace Lambda
{
	class IWindow;
	class ISystemEventListener;

	//-------
	//ISystem
	//-------

	class ISystem
	{
	public:
		LAMBDA_INTERFACE(ISystem);

		virtual IWindow* CreateWindow(const char* pTitle, uint32 width, uint32 height) = 0;

		virtual void ProcessEvents() = 0;

		virtual void AddListener(ISystemEventListener* pListener) = 0;
		virtual void RemoveListener(ISystemEventListener* pListener) = 0;
	};

	//------
	//System
	//------

	class LAMBDA_API System
	{
	public:
		static void Initialize();
		static void Release();

		inline static IWindow* CreateWindow(const char* pTitle, uint32 width, uint32 height) { return s_pInstance->CreateWindow(pTitle, width, height); }

		inline static void ProcessEvents() { s_pInstance->ProcessEvents(); }

		inline static void AddListener(ISystemEventListener* pListener)	   { s_pInstance->AddListener(pListener); };
		inline static void RemoveListener(ISystemEventListener* pListener) { s_pInstance->RemoveListener(pListener); };
	protected:
		static ISystem* s_pInstance;
	};
}
