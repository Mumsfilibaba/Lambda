#pragma once
#include "LambdaCore.h"

#ifdef CreateWindow
	#undef CreateWindow
#endif

namespace Lambda
{
	struct SSystemEvent;

	class IEngine;
	class IWindow;
	class ISystemEventListener;

	//-------
	//ISystem
	//-------
	class LAMBDA_API ISystem
	{
	public:
		LAMBDA_INTERFACE(ISystem);

		virtual bool Initialize() = 0;

		virtual bool CreateWindow(const char* pTitle, uint32 nWidth, uint32 nHeight) = 0;

		virtual void ProcessSystemEvents() = 0;
		virtual void AddEventListener(ISystemEventListener* pListener) = 0;
		virtual void RemoveEventListener(ISystemEventListener* pListener) = 0;

		virtual void Release() = 0;

		virtual const IWindow* GetWindow() const = 0;
	public:
		static ISystem* Create(IEngine* pEngine);
	};
}