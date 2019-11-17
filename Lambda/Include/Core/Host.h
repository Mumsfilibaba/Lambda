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


		virtual void AddEventListener(IHostEventListener* pListener)
		{
			LAMBDA_ASSERT(pListener != nullptr, "[LAMBDA ENGINE] pListener cannot be nullptr");
			m_pEventListeners.emplace_back(pListener);
		}
	protected:
		virtual void OnQuit()
		{
			for (auto pListener : m_pEventListeners)
				pListener->OnHostQuit();
		}
	protected:
		std::vector<IHostEventListener*> m_pEventListeners;
	public:
		static Host* Create();
	};
}