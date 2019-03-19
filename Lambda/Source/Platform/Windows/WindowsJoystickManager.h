#pragma once
#include <System/JoystickManager.h>

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

		virtual void UpdateJoystickState() override final;

	private:
		XINPUT_STATE m_ControllerState[XUSER_MAX_COUNT];
	};
}
#endif