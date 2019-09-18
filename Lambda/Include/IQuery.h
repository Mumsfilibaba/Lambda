#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
    //------
    //IQuery
    //------
    
    class IQuery
    {
    public:
        LAMBDA_INTERFACE(IQuery);
        
        IQuery() = default;
        ~IQuery() = default;
        
        virtual void* GetNativeHandle() const = 0;
    };
}
