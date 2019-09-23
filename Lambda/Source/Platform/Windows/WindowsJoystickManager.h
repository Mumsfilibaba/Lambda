#pragma once
#include "System/JoystickManager.h"
#include "Time/Clock.h"

#if defined(LAMBDA_PLAT_WINDOWS)
#include <Xinput.h>

namespace Lambda
{
	//----------------------
	//WindowsJoystickManager
	//----------------------

	class WindowsJoystickManager final : public JoystickManager
	{
	public:
		LAMBDA_NO_COPY(WindowsJoystickManager);

		WindowsJoystickManager();
		~WindowsJoystickManager() = default;

		virtual void InternalOnUpdate() override final;
		virtual void InternalSetPollrate(const Timestep& time) override final;
		virtual Timestep InternalGetPollrate() const override final;

	private:
		Clock m_PollingTimer;
		Timestep m_PollRate;
		Timestep m_CurrentPollRate;
		XINPUT_STATE m_ControllerState[XUSER_MAX_COUNT];
	};
}
#endif
