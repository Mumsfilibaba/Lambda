#pragma once
#include "IDeviceObject.h"

namespace Lambda
{
    //----------
    //BufferDesc
    //----------
    
	struct BufferDesc
	{
        const char* pName      = nullptr;
		ResourceUsage Usage    = RESOURCE_USAGE_UNKNOWN;
		uint32 Flags           = BUFFER_FLAGS_NONE;
		uint64 SizeInBytes     = 0;
		uint32 StrideInBytes   = 0;
	};

    //-------
    //IBuffer
    //-------

	class LAMBDA_API IBuffer : public IDeviceObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(IBuffer);

		virtual void Map(void** ppMem) = 0;
		virtual void Unmap() = 0;
        virtual void* GetNativeHandle() const = 0;
        virtual const BufferDesc& GetDesc() const = 0;
	};
}
