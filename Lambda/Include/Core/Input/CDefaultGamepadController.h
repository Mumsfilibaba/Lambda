#pragma once
#include "IGamepadController.h"

namespace Lambda
{
	//-------------------------
	//CDefaultGamepadController 
	//-------------------------

	class CDefaultGamepadController final : public IGamepadController
	{
	public:
		CDefaultGamepadController();
		~CDefaultGamepadController() = default;

		virtual CGamepadState GetGamepadState(EGamepad gamepadID) const override final;
	private:
		CGamepadState m_GamepadState;
	};
}