#pragma once
#include "LambdaCore.h"
#include "Singleton.h"

#include "Core/Event/ISystemEventListener.h"

#include <vector>

#if defined(CreateWindow)
	#undef CreateWindow
#endif

namespace Lambda
{
	class CWindow;

	//------------
	//CApplication
	//------------

	class LAMBDA_API CApplication : public CSingleton<CApplication>
	{
	public:
		CApplication();
		~CApplication();

		virtual void AddEventListener(ISystemEventListener* pListener);
		virtual void RemoveEventListener(ISystemEventListener* pListener);
        
        virtual void SetDisplayMode(EDisplayMode mode);
        virtual void SetDisplayMetrics(uint32 width, uint32 height);
        virtual void GetDisplayMetrics(uint32& width, uint32& height) const;
        
        virtual CWindow* GetForegroundWindow() const;
        
		static CWindow* CreateWindow() { return nullptr; }
		static CApplication* CreateApplication() { return nullptr; }
		static void Initialize() {}
		static void PollEvents() {}
		static void Release() {}
	private:
		std::vector<ISystemEventListener*> m_EventListeners;
	};
}
