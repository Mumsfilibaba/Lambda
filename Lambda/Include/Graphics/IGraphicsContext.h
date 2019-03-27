#pragma once
#include "../IObject.h"

namespace Lambda
{
	class LAMBDA_API IGraphicsContext : public IObject
	{
	public:
		LAMBDA_INTERFACE(IGraphicsContext);

		IGraphicsContext() = default;
		~IGraphicsContext() = default;

	public:
		static IGraphicsContext* Create();
	};
}