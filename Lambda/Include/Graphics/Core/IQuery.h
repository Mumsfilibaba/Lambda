#pragma once
#include "IDeviceObject.h"

namespace Lambda
{
    //---------
    //QueryType
    //---------
    
    enum QueryType : uint32
    {
        QUERY_TYPE_UNKNOWN      = 0,
        QUERY_TYPE_TIMESTAMP    = 1,
    };
    
    //---------
    //QueryDesc
    //---------
    
    struct QueryDesc
    {
        QueryType Type     = QUERY_TYPE_UNKNOWN;
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
        virtual const QueryDesc& GetDesc() const = 0;
    };
}
