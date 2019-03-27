#pragma once
#include <Defines.h>
#include <Types.h>

namespace Lambda
{
	class LAMBDA_API JoystickManager
	{
	public:
		LAMBDA_NO_COPY(JoystickManager);

		JoystickManager() = default;
		virtual ~JoystickManager() = default;

	private:
		virtual void UpdateJoystickState() = 0;

	public:
		static void OnUpdate();

	private:
		static JoystickManager* Create();

	private:
		static std::unique_ptr<JoystickManager> s_Instance;
	};
}