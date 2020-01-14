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
	//EDisplayMode
	//------------

	enum class EDisplayMode
	{
		DISPLAY_MODE_WINDOWED				= 0,
		DISPLAY_MODE_FULLSCREEN				= 1,
		DISPLAY_MODE_BORDERLESS_FULLSCREEN	= 2
	};

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

		static CWindow* CreateWindow() { return nullptr; }
		static CApplication* CreateApplication() { return nullptr; }
		static void Initialize() {}
		static void PollEvents() {}
		static void Release() {}
	private:
		std::vector<ISystemEventListener*> m_EventListeners;
	};
}