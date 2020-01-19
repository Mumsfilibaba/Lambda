#pragma once
#include "LambdaCore.h"
#include "Window.h"

#include "Core/Engine/Singleton.h"
#include "Core/Event/ISystemEventListener.h"

#include <vector>

#if defined(CreateWindow)
	#undef CreateWindow
#endif

namespace Lambda
{
	//------------
	//CApplication
	//------------

	class LAMBDA_API CApplication : public CSingleton<CApplication>
	{
	protected:
		CApplication();
	public:
		~CApplication();

		virtual CWindow* CreateWindow(const SWindowProps&) { return nullptr; }
		virtual CWindow* GetCurrentWindow() const {	return nullptr; }

		virtual void AddEventListener(ISystemEventListener* pListener);
		virtual void RemoveEventListener(ISystemEventListener* pListener);
        
		virtual bool DispatchEvent(const SSystemEvent& event) const;
		virtual void Terminate(int32) const {}
        
		static void PreInit()		{}
		static void Init()			{}
		static void ProcessEvents() {}
		static void Release()		{}

		static CApplication* CreateApplication() { return nullptr; }
	private:
		std::vector<ISystemEventListener*> m_EventListeners;
	};
}
