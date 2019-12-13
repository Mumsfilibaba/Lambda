#pragma once
#include "Core/Input/CMouseControllerBase.h"
#if defined(LAMBDA_PLAT_WINDOWS)

namespace Lambda
{
	//-----------------------
	//CWindowsMouseController
	//-----------------------

	class CWindowsMouseController final : public CMouseControllerBase
	{
	public:
		CWindowsMouseController();
		~CWindowsMouseController() = default;

		virtual Point GetPosition() const override final;
		virtual void SetPosition(const Point& position) override final;
	};
}
#endif