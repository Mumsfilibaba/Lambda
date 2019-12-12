#pragma once
#include "Core/Input/CEventMouseController.h"
#if defined(LAMBDA_PLAT_WINDOWS)

namespace Lambda
{
	//-----------------------
	//CWindowsMouseController
	//-----------------------

	class CWindowsMouseController final : public CEventMouseController
	{
	public:
		CWindowsMouseController();
		~CWindowsMouseController() = default;

		virtual Point GetPosition() const override final;
		virtual void SetPosition(const Point& position) override final;
	};
}
#endif