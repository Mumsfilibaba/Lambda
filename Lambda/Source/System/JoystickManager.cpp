#include <LambdaPch.h>
#include <System/JoystickManager.h>

namespace Lambda
{
	std::unique_ptr<JoystickManager> JoystickManager::s_Instance = std::unique_ptr<JoystickManager>(JoystickManager::Create());

	void JoystickManager::OnUpdate()
	{
		s_Instance->UpdateJoystickState();
	}
}