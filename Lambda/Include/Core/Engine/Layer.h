#pragma once
#include "LambdaCore.h"

#include "Core/Time/Time.h"

namespace Lambda
{
	struct SystemEvent;

	//-----
	//Layer
	//-----

	class LAMBDA_API Layer
	{
	public:
		Layer(const char* pName);
		~Layer() = default;

		LAMBDA_NO_COPY(Layer);

		virtual void OnUpdate(const Time& deltaTime);
		virtual bool OnSystemEvent(const SystemEvent& event);
	};
}