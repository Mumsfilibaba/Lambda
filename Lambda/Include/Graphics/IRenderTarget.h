#pragma once
#include <IObject.h>

namespace Lambda
{
	class IRenderTarget : public IObject
	{
	public:
		LAMBDA_INTERFACE(IRenderTarget);

		IRenderTarget() = default;
		~IRenderTarget() = default;
	};
}