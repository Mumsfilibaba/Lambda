#pragma once
#include "Defines.h"
#include "Types.h"

#define IOBJECT_IMPLEMENT_RELEASE(refCounter)	\
{												\
	uint32 refs = --refCounter;					\
	if (refs < 1)	{ delete this; }			\
	return refs;								\
}												\


namespace Lambda
{
    //-------
    //IObject
    //-------
    
	//Baseclass for referencecounted objects. Classes that inherits from IObject should not
	//have a virtual constructor since they should be deleted with the Release-function
	class LAMBDA_API IObject
	{
	public:
		LAMBDA_INTERFACE(IObject);

		IObject() = default;
		~IObject() = default;

		virtual uint32 Release()    = 0;
		virtual uint32 AddRef()     = 0;
	};
}
