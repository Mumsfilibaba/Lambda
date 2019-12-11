#include "LambdaPch.h"
#include "CBaseWindow.h"

namespace Lambda
{
	//-----------
	//CBaseWindow
	//-----------

	CBaseWindow::CBaseWindow()
		: m_pEventCallback(nullptr)
	{
	}


	CBaseWindow::~CBaseWindow()
	{
		SafeDelete(m_pEventCallback);
	}


	void CBaseWindow::SetEventCallback(IEventCallback* pEventCallback)
	{
		SafeDelete(m_pEventCallback);
		m_pEventCallback = pEventCallback;
	}
	

	bool CBaseWindow::DispatchEvent(const CEvent & event)
	{
		if (m_pEventCallback)
			return m_pEventCallback->Call(event);

		return false;
	}
}