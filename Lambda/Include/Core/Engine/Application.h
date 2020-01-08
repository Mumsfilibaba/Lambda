#pragma once
#include "LambdaCore.h"

#ifdef CreateWindow
	#undef CreateWindow
#endif

namespace Lambda
{
	struct SSystemEvent;

	class IWindow;
	class ISystemEventListener;

	//------------
	//IApplication
	//------------
	class LAMBDA_API IApplication
	{
	public:
		LAMBDA_INTERFACE(IApplication);

		virtual bool CreateWindow(const char* pTitle, uint32 nWidth, uint32 nHeight) = 0;

		virtual void ProcessEvents() = 0;
		virtual void AddListener(ISystemEventListener* pListener) = 0;
		virtual void RemoveListener(ISystemEventListener* pListener) = 0;

		virtual void Release() = 0;

		virtual const IWindow* GetWindow() const = 0;
	};

	//------------
	//CApplication
	//------------
	class CApplication
	{
	public:
		LAMBDA_STATIC_CLASS(CApplication);

		static void Initialize();
		static void Release();

		/*////////////////////////////////////////////////////////////////////////////////////////////////*/
		static bool CreateWindow(const char* pTitle, uint32 nWidth, uint32 nHeight)
		{
			return s_pApplication->CreateWindow(pTitle, nWidth, nHeight);
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////////*/
		static void ProcessEvents()
		{
			s_pApplication->ProcessEvents();
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////////*/
		static void AddListener(ISystemEventListener* pListener)
		{
			s_pApplication->AddListener(pListener);
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////////*/
		static void RemoveListener(ISystemEventListener* pListener)
		{
			s_pApplication->RemoveListener(pListener);
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////////*/
		static const IWindow* GetWindow()
		{
			return s_pApplication->GetWindow();
		}

		/*//////////////////*/
		static IApplication* Get()
		{
			return s_pApplication;
		}
	private:
		static IApplication* s_pApplication;
	};
}
