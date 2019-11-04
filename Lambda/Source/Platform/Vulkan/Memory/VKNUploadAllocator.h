#pragma once
#include "VKNDeviceAllocator.h"

namespace Lambda
{
	//-------------------
	//VKNUploadAllocation
	//-------------------

	class VKNUploadPage;

	struct VKNUploadAllocation
	{
		VKNUploadPage* pPage  = nullptr;
		VkDeviceSize   Offset = 0;
	};

	//-------------
	//VKNUploadPage
	//-------------

	class VKNUploadPage
	{
	public:
		LAMBDA_NO_COPY(VKNUploadPage);

		VKNUploadPage(VKNDevice* pVkDevice, VkDeviceSize sizeInBytes);
		~VKNUploadPage() = default;

		VKNUploadAllocation Allocate(VkDeviceSize sizeInBytes, VkDeviceSize alignment);
		void Destroy(VKNDevice* pVkDevice);


		_forceinline void Reset()
		{
			m_OffsetInBytes = 0;
		}


		_forceinline VkDeviceSize GetSizeLeft() const
		{ 
			return m_SizeInBytes - m_OffsetInBytes;
		}


		_forceinline uint8* GetHostMemory() const
		{
			return m_Memory.pHostMemory;
		}


		_forceinline VkBuffer GetVkBuffer() const
		{
			return m_VkBuffer;
		}
	private:
		void Init(VKNDevice* pVkDevice);
	private:
		VKNAllocation m_Memory;
		VkBuffer m_VkBuffer;
		VkDeviceSize m_SizeInBytes;
		VkDeviceSize m_OffsetInBytes;
	};


	//------------------
	//VKNUploadAllocator
	//------------------

	class VKNUploadAllocator final
	{
	public:
		LAMBDA_NO_COPY(VKNUploadAllocator);

		VKNUploadAllocator(VKNDevice* pVkDevice, VkDeviceSize sizeInBytes = MB(1));
		~VKNUploadAllocator();

		VKNUploadAllocation Allocate(VkDeviceSize sizeInBytes, VkDeviceSize alignment);
		void Reset();
	private:
		std::vector<VKNUploadPage*> m_DiscardedPages;		
		VKNDevice* m_pVkDevice;
		VKNUploadPage* m_pCurrentPage;
	};
}