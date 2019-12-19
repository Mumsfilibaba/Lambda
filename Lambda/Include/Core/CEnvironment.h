#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	//------------
	//CEnvironment
	//------------

	class CEnvironment
	{
	public:
		virtual void ProcessEvents() = 0;
		virtual void Release() = 0;
	};
}