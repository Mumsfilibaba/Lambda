#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	typedef uint32 RefCountValue;

    //-------
    //IObject
    //-------
    
	//Interface for referencecounted objects. Classes that inherits from IObject should not
	//have a virtual constructor since they should be deleted with the Release-function
	class LAMBDA_API IObject
	{
	public:
		LAMBDA_INTERFACE(IObject);

		virtual RefCountValue Release() = 0;
		virtual RefCountValue AddRef()  = 0;
	};
}
