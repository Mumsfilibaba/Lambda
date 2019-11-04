#pragma once
#include "Defines.h"
#include "Types.h"
#include "Time/Timestep.h"
#include "Utilities/Singleton.h"
#include <memory>

namespace Lambda
{
    //--------------
    //GamePadManager
    //--------------
    
	class LAMBDA_API GamePadManager : public Singleton<GamePadManager>
	{
	public:
		LAMBDA_NO_COPY(GamePadManager);

		GamePadManager() : Singleton<GamePadManager>() {}
		virtual ~GamePadManager() = default;
	private:
		virtual void InternalOnUpdate() = 0;
		virtual void InternalSetPollrate(const Timestep& time) = 0;
		virtual Timestep InternalGetPollrate() const = 0;
	public:
		_forceinline static void GamePadManager::OnUpdate()
		{
			Get().InternalOnUpdate();
		}


		_forceinline static void GamePadManager::SetPollrate(const Timestep& time)
		{
			Get().InternalSetPollrate(time);
		}


		_forceinline static Timestep GamePadManager::GetPollrate()
		{
			return Get().InternalGetPollrate();
		}
	public:
		static GamePadManager* Create();
	};
}
