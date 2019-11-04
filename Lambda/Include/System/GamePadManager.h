#pragma once
#include "Defines.h"
#include "Types.h"
#include "Time/Timestep.h"
#include <memory>

namespace Lambda
{
    //--------------
    //GamePadManager
    //--------------
    
	class LAMBDA_API GamePadManager
	{
	public:
		LAMBDA_NO_COPY(GamePadManager);

		GamePadManager() = default;
		virtual ~GamePadManager() = default;
	private:
		virtual void InternalOnUpdate() = 0;
		virtual void InternalSetPollrate(const Timestep& time) = 0;
		virtual Timestep InternalGetPollrate() const = 0;
	public:
		static void OnUpdate();
		static void SetPollrate(const Timestep& time);
		static Timestep GetPollrate();
	private:
		static GamePadManager* Create();
	private:
		static std::unique_ptr<GamePadManager> s_Instance;
	};
}
