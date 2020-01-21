#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	class LAMBDA_API CGenericApplication
	{
	public:
		LAMBDA_STATIC_CLASS(CGenericApplication);

		static void Init() {}
		static void PollEvents() {}
	};
}