#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
#include "CWindowsMouseController.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <Windows.h>

namespace Lambda
{
	//----------------------
	//CWindowMouseController
	//----------------------

	CWindowsMouseController::CWindowsMouseController()
		: CMouseControllerBase(),
		m_bIsMouseVisible(true)
	{
	}


	bool CWindowsMouseController::IsMouseVisible() const
	{
		return m_bIsMouseVisible;
	}


	void CWindowsMouseController::SetMouseVisisble(bool bVisible)
	{
		if (m_bIsMouseVisible != bVisible)
		{
			m_bIsMouseVisible = bVisible;
			if (bVisible)
				ShowCursor(TRUE);
			else
				ShowCursor(false);
		}
	}

	
	void CWindowsMouseController::SetPosition(const Point& position)
	{
		SetCursorPos(int(position.x), int(position.y));
	}

	
	Point CWindowsMouseController::GetPosition() const
	{
		POINT p = {};
		GetCursorPos(&p);

		return Point(p.x, p.y);
	}
}

#endif
