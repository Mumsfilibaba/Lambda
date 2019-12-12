#pragma once
#include "IGamepadController.h"

namespace Lambda
{
	//-----------------------
	//CDummyGamepadController 
	//-----------------------

	class CDummyGamepadController final : public IGamepadController
	{
	public:
		CDummyGamepadController() = default;
		~CDummyGamepadController() = default;

		virtual CGamepadState GetGamepadState(EGamepad gamepadID) const override final;
	};
}