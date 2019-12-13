#include "LambdaPch.h"
#include "CWindowBase.h"

namespace Lambda
{
	//-----------
	//CWindowBase
	//-----------

	CWindowBase::CWindowBase()
		: m_pEventCallback(nullptr)
	{
	}


	CWindowBase::~CWindowBase()
	{
		SafeDelete(m_pEventCallback);
	}


	void CWindowBase::SetEventCallback(IEventCallback* pEventCallback)
	{
		SafeDelete(m_pEventCallback);
		m_pEventCallback = pEventCallback;
	}
	

	bool CWindowBase::DispatchEvent(const CEvent& event)
	{
		if (m_pEventCallback)
			return m_pEventCallback->Call(event);

		return false;
	}
}