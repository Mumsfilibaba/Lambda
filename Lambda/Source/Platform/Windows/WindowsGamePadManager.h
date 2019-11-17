#pragma once
#include "Core/GamePadManager.h"
#include "Time/Clock.h"

#if defined(LAMBDA_PLAT_WINDOWS)
#include <Xinput.h>

namespace Lambda
{
	//----------------------
	//WindowsJoystickManager
	//----------------------

	class WindowsGamePadManager final : public GamePadManager
	{
	public:
		LAMBDA_NO_COPY(WindowsGamePadManager);

		WindowsGamePadManager();
		~WindowsGamePadManager() = default;

		virtual void InternalOnUpdate() override final;
		virtual void InternalSetPollrate(const Timestep& time) override final;
		virtual Timestep InternalGetPollrate() const override final;
	private:
		XINPUT_STATE m_ControllerState[XUSER_MAX_COUNT];
		Clock m_PollingTimer;
		Timestep m_PollRate;
		Timestep m_CurrentPollRate;
	};
}
#endif
