#include <LambdaPch.h>
#include <System/JoystickManager.h>

namespace Lambda
{
	std::unique_ptr<JoystickManager> JoystickManager::s_Instance = std::unique_ptr<JoystickManager>(JoystickManager::Create());

	void JoystickManager::OnUpdate()
	{
		s_Instance->InternalOnUpdate();
	}

	void JoystickManager::SetPollrate(const Time& time)
	{
		s_Instance->InternalSetPollrate(time);
	}

	Time JoystickManager::GetPollrate()
	{
		return s_Instance->InternalGetPollrate();
	}
}