#pragma once
#include "Core/Input/CMouseControllerBase.h"
#if defined(LAMBDA_PLAT_WINDOWS)

namespace Lambda
{
	class CEnvironment;

	//-----------------------
	//CWindowsMouseController
	//-----------------------

	class CWindowsMouseController final : public CMouseControllerBase
	{
	public:
		CWindowsMouseController(CEnvironment* pEnvironment);
		~CWindowsMouseController() = default;

		virtual bool IsMouseVisible() const override final;
		virtual void SetMouseVisisble(bool bVisible) override final;
		virtual void SetPosition(const Point& position) override final;
		virtual Point GetPosition() const override final;
	private:
		bool m_bIsMouseVisible;
	};
}
#endif