#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	class IWindow;

	//-------
	//ISystem
	//-------
	class LAMBDA_API ISystem
	{
	public:
		LAMBDA_INTERFACE(ISystem);

		virtual void ProcessSystemEvents() = 0;
		virtual void Release() = 0;

		virtual IWindow* GetWindow() = 0;
	public:
		static ISystem* Create();
	};
}