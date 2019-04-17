#pragma once
#include <IObject.h>

namespace Lambda
{
	struct BufferDesc
	{
		ResourceUsage Usage = RESOURCE_USAGE_UNKNOWN;
		BufferFlags Flags = BUFFER_FLAGS_NONE;
		uint32 StrideInBytes = 0;
		uint32 SizeInBytes = 0;
	};


	class IBuffer : public IObject
	{
	public:
		LAMBDA_INTERFACE(IBuffer);

		IBuffer() = default;
		~IBuffer() = default;

		virtual void Map(void** ppMem) = 0;
		virtual void Unmap() = 0;

		virtual BufferDesc GetDesc() const = 0;
	};
}