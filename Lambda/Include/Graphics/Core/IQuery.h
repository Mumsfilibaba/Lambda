#pragma once
#include "IDeviceObject.h"

namespace Lambda
{
    //----------
    //EQueryType
    //----------
    
    enum EQueryType : uint32
    {
        QUERY_TYPE_UNKNOWN      = 0,
        QUERY_TYPE_TIMESTAMP    = 1,
    };
    
    //----------
    //SQueryDesc
    //----------
    
    struct SQueryDesc
    {
        EQueryType Type    = QUERY_TYPE_UNKNOWN;
        uint32 QueryCount  = 0;
    };
    
    //------
    //IQuery
    //------
    
    class IQuery : public IDeviceObject
    {
    public:
		LAMBDA_IOBJECT_INTERFACE(IQuery);
        
        virtual void GetResults(uint64* pResults, uint32 numResults, uint32 startQuery) = 0;
        virtual void* GetNativeHandle() const = 0;
        virtual const SQueryDesc& GetDesc() const = 0;
    };
}
