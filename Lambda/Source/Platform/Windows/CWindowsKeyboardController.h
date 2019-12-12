#pragma once
#include "Core/Input/IKeyboardController.h"
#if defined(LAMBDA_PLAT_WINDOWS)

namespace Lambda
{
	//--------------------------
	//CWindowsKeyboardController
	//--------------------------

	class CWindowsKeyboardController final : public IKeyboardController
	{
	public:
		CWindowsKeyboardController();
		~CWindowsKeyboardController() = default;

		virtual bool IsKeyUp(EKey key) const override final;
		virtual bool IsKeyDown(EKey key) const override final;
		virtual CKeyboardState GetKeyboardState() const override final;
	};
}

#endif