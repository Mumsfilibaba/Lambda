#pragma once
#include "IObject.h"

namespace Lambda
{
	struct BufferDesc
	{
        const char*     pName           = nullptr;
		ResourceUsage   Usage           = RESOURCE_USAGE_UNKNOWN;
		uint32          Flags           = BUFFER_FLAGS_NONE;
		uint64          SizeInBytes     = 0;
		uint32          StrideInBytes   = 0;
	};


	class LAMBDA_API IBuffer
	{
	public:
		LAMBDA_INTERFACE(IBuffer);

		IBuffer() = default;
		~IBuffer() = default;

		virtual void Map(void** ppMem) = 0;
		virtual void Unmap() = 0;

        virtual void*       GetNativeHandle() const = 0;
        virtual BufferDesc  GetDesc() const = 0;
	};
}
