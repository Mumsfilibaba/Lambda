#include "LambdaPch.h"
#include "CWindowsMouseController.h"
#if defined(LAMBDA_PLAT_WINDOWS)

namespace Lambda
{
	//----------------------
	//CWindowMouseController
	//----------------------

	CWindowsMouseController::CWindowsMouseController()
		: CMouseControllerBase()
	{
	}

	
	Point CWindowsMouseController::GetPosition() const
	{
		POINT p = {};
		GetCursorPos(&p);

		return Point(p.x, p.y);
	}


	void CWindowsMouseController::SetPosition(const Point& position)
	{
		SetCursorPos(int(position.x), int(position.y));
	}
}

#endif
