#pragma once
#include <System/JoystickManager.h>
#include <System/Clock.h>

#if defined(LAMBDA_PLAT_WINDOWS)
#include <Xinput.h>

namespace Lambda
{
	class WindowsJoystickManager final : public JoystickManager
	{
	public:
		LAMBDA_NO_COPY(WindowsJoystickManager);

		WindowsJoystickManager();
		~WindowsJoystickManager();

		virtual void InternalOnUpdate() override final;
		virtual void InternalSetPollrate(const Time& time) override final;
		virtual Time InternalGetPollrate() const override final;

	private:
		Clock m_PollingTimer;
		Time m_PollRate;
		Time m_CurrentPollRate;
		XINPUT_STATE m_ControllerState[XUSER_MAX_COUNT];
	};
}
#endif