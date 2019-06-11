#pragma once
#include "Defines.h"
#include "Types.h"
#include "Time.hpp"
#include <memory>

namespace Lambda
{
	class LAMBDA_API JoystickManager
	{
	public:
		LAMBDA_NO_COPY(JoystickManager);

		JoystickManager() = default;
		virtual ~JoystickManager() = default;

	private:
		virtual void InternalOnUpdate() = 0;
		virtual void InternalSetPollrate(const Time& time) = 0;
		virtual Time InternalGetPollrate() const = 0;

	public:
		static void OnUpdate();
		static void SetPollrate(const Time& time);
		static Time GetPollrate();

	private:
		static JoystickManager* Create();

	private:
		static std::unique_ptr<JoystickManager> s_Instance;
	};
}
