#pragma once
#include "Core/CEnvironment.h"
#include "Core/Input/EKey.h"

#define GET_WINDOWS_ENVIRONMENT (Lambda::CWindowsEnvironment&)Lambda::CEnvironment::Get

namespace Lambda
{
	class CWindowsWindow;
	class CWindowMoveEvent;
	class CWindowClosedEvent;
	class CWindowResizeEvent;
	class CWindowFocusChangedEvent;

	//------------------
	//WindowsEnvironment
	//------------------

	class CWindowsEnvironment final : public CEnvironment
	{
	public:
		CWindowsEnvironment() = default;
		~CWindowsEnvironment() = default;

		EKey ConvertKeyFromWindows(uint32 key);
		uint32 ConvertKeyFromEngine(EKey key);

		virtual void Initialize() override final;
		virtual void ProcessEvents() override final;
		virtual void Release() override final;
		virtual void PrintF(const char* pFormat, ...) override final;
	private:
		void InitializeLookUpTables();

		bool OnWindowClose(const CWindowClosedEvent&);
		bool OnWindowMove(const CWindowMoveEvent& event);
		bool OnWindowResize(const CWindowResizeEvent& event);
		bool OnWindowFocusChanges(const CWindowFocusChangedEvent& event);
		
		virtual bool OnEvent(const CEvent& event) override final;
	private:
		CWindowsWindow* m_pWindow;
		EKey m_WindowsToLambdaKeyTable[256];
		uint32 m_LambdaToWindowsKeyTable[KEY_LAST+1];
	};
}