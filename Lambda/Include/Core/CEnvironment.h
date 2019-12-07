#pragma once
#include "LambdaCore.h"
#include "CSingleton.h"
#include "Event/ISystemEventListener.h"

namespace Lambda
{
	//------------
	//CEnvironment
	//------------

	class CEnvironment : public CSingleton<CEnvironment>
	{
	public:
		LAMBDA_INTERFACE(CEnvironment);

		virtual void Init() = 0;
		virtual void ProcessEvents() = 0;
		virtual void Release() = 0;
		virtual void PrintF(const char* pFormat, ...) = 0;

		virtual void AddEventListener(ISystemEventListener* pListener)
		{
			LAMBDA_ASSERT_PRINT(pListener != nullptr, "[LAMBDA ENGINE] pListener cannot be nullptr");
			m_pEventListeners.emplace_back(pListener);
		}
	protected:
		virtual void OnEvent(const SSystemEvent& event)
		{
			for (auto pListener : m_pEventListeners)
				pListener->OnEvent(event);
		}
	protected:
		std::vector<ISystemEventListener*> m_pEventListeners;
	public:
		static CEnvironment* Create();
	};
}
