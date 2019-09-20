#pragma once
#include "Defines.h"
#include "Types.h"
#include "Timestep.h"
#include <memory>

namespace Lambda
{
    //---------------
    //JoystickManager
    //---------------
    
	class LAMBDA_API JoystickManager
	{
	public:
		LAMBDA_NO_COPY(JoystickManager);

		JoystickManager() = default;
		virtual ~JoystickManager() = default;

	private:
		virtual void        InternalOnUpdate() = 0;
		virtual void        InternalSetPollrate(const Timestep& time) = 0;
		virtual Timestep    InternalGetPollrate() const = 0;

	public:
		static void     OnUpdate();
		static void     SetPollrate(const Timestep& time);
		static Timestep GetPollrate();

	private:
		static JoystickManager* Create();

	private:
		static std::unique_ptr<JoystickManager> s_Instance;
	};
}
