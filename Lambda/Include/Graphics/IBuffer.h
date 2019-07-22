#pragma once
#include "IObject.h"

namespace Lambda
{
    //Describe GPU-buffer
	struct BufferDesc
	{
		ResourceUsage Usage     = RESOURCE_USAGE_UNKNOWN;
		uint32 Flags            = BUFFER_FLAGS_NONE;
		uint32 StrideInBytes    = 0;
		uint32 SizeInBytes      = 0;
	};


    //GPU-Buffer interface
	class LAMBDA_API IBuffer
	{
	public:
		LAMBDA_INTERFACE(IBuffer);

		IBuffer() = default;
		~IBuffer() = default;

		virtual void Map(void** ppMem) = 0;
		virtual void Unmap() = 0;

        virtual void* GetNativeHandle() const = 0;
        virtual uint64 GetSizeInBytes() const = 0;
        virtual BufferDesc GetDesc() const = 0;
	};
}
