#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
	class LAMBDA_API IObject
	{
	public:
		LAMBDA_INTERFACE(IObject);

		IObject() = default;
		~IObject() = default;

		virtual uint32 Release() = 0;
		virtual uint32 AddRef() = 0; 
	};
}