#include "LambdaPch.h"
#include "System/GamePadManager.h"

namespace Lambda
{
	//--------------
	//GamePadManager
	//--------------

	std::unique_ptr<GamePadManager> GamePadManager::s_Instance = std::unique_ptr<GamePadManager>(GamePadManager::Create());

	void GamePadManager::OnUpdate()
	{
		s_Instance->InternalOnUpdate();
	}

    
	void GamePadManager::SetPollrate(const Timestep& time)
	{
		s_Instance->InternalSetPollrate(time);
	}

    
	Timestep GamePadManager::GetPollrate()
	{
		return s_Instance->InternalGetPollrate();
	}
}
