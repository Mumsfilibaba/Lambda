#pragma once
#include "VKNDeviceAllocator.h"

namespace Lambda
{
	//-------------------
	//VKNUploadAllocation
	//-------------------

	struct VKNUploadAllocation
	{
		VkBuffer Buffer		  = VK_NULL_HANDLE;
		uint64	 DeviceOffset = 0;
		uint64	 SizeInBytes  = 0;
		void*	 pHostMemory  = nullptr;
	};

	//---------------
	//VKNUploadAllocator
	//---------------

	class VKNUploadAllocator final
	{
	public:
		LAMBDA_NO_COPY(VKNUploadAllocator);

		VKNUploadAllocator(VKNDevice* pDevice, uint64 sizeInBytes);
		~VKNUploadAllocator();

		VKNUploadAllocation Allocate(uint64 bytesToAllocate, uint64 alignment);
		void Reset();

		inline VkBuffer GetVkBuffer() const { return m_Buffer; }
	private:
		bool Init(uint64 sizeInBytes);
		void Reallocate(uint64 sizeInBytes);
	private:
		VKNDevice* m_pDevice;
		VkBuffer  m_Buffer;
		VKNAllocation m_Memory;
		uint64    m_Offset;
	};
}