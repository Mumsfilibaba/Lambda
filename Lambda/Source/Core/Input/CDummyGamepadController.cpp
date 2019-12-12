#include "LambdaPch.h"
#include "Core/Input/CDummyGamepadController.h"

namespace Lambda
{
	//-----------------------
	//CDummyGamepadController 
	//-----------------------

	CGamepadState CDummyGamepadController::GetGamepadState(EGamepad) const
	{
		//This is a dummy gampad controller so just return a state
		return CGamepadState();
	}
}