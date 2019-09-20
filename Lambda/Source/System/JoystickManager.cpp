#include "LambdaPch.h"
#include "System/JoystickManager.h"

namespace Lambda
{
	std::unique_ptr<JoystickManager> JoystickManager::s_Instance = std::unique_ptr<JoystickManager>(JoystickManager::Create());

    
	void JoystickManager::OnUpdate()
	{
		s_Instance->InternalOnUpdate();
	}

    
	void JoystickManager::SetPollrate(const Timestep& time)
	{
		s_Instance->InternalSetPollrate(time);
	}

    
	Timestep JoystickManager::GetPollrate()
	{
		return s_Instance->InternalGetPollrate();
	}
}
