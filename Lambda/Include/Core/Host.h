#pragma once
#include "LambdaCore.h"
#include "Utilities/Singleton.h"
#include "IHostEventListener.h"
#include "IWindowEventListener.h"

namespace Lambda
{
	//----
	//Host
	//----

	class Host : public Singleton<Host>
	{
	public:
		LAMBDA_INTERFACE(Host);

		virtual void Init() = 0;
		virtual void ProcessEvents() = 0;
		virtual void Release() = 0;
		virtual void PrintF(const char* pFormat, ...) = 0;

		virtual void AddEventListener(IHostEventListener* pListener)
		{
			LAMBDA_ASSERT_PRINT(pListener != nullptr, "[LAMBDA ENGINE] pListener cannot be nullptr");
			m_pEventListeners.emplace_back(pListener);
		}
	protected:
		virtual void OnQuit(int32 exitCode)
		{
			for (auto pListener : m_pEventListeners)
				pListener->OnHostQuit(exitCode);
		}
	protected:
		std::vector<IHostEventListener*> m_pEventListeners;
	public:
		static Host* Create();
	};
}