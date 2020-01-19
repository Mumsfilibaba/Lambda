#include "LambdaPch.h"

#include "Platform/Application.h"

namespace Lambda
{
	//------------
	//CApplication
	//------------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CApplication::CApplication()
		: CSingleton<CApplication>(),
		m_EventListeners()
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CApplication::~CApplication()
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CApplication::AddEventListener(ISystemEventListener* pListener)
	{
		m_EventListeners.emplace_back(pListener);
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CApplication::RemoveEventListener(ISystemEventListener* pListener)
	{
		for (auto it = m_EventListeners.begin(); it != m_EventListeners.end(); it++)
		{
			if (pListener == *it)
			{
				it = m_EventListeners.erase(it);
				return;
			}
		}
	}
	
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool CApplication::DispatchEvent(const SSystemEvent& event) const
	{
		bool bHandled = false;
		for (auto listener : m_EventListeners)
		{
			if (listener->OnSystemEvent(event))
				bHandled = true;
		}

		return bHandled;
	}
}