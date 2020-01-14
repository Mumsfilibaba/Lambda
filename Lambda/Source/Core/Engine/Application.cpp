#include "LambdaPch.h"

#include "Core/Engine/Application.h"

namespace Lambda
{
	//------------
	//CApplication
	//------------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CApplication::CApplication()
		:  m_EventListeners()
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
				m_EventListeners.erase(it);
				return;
			}
		}
	}
}