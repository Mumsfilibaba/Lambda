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

		VKNUploadPage(VKNDevice* pDevice, VkDeviceSize sizeInBytes);
		~VKNUploadPage() = default;

		VKNUploadAllocation Allocate(VkDeviceSize sizeInBytes, VkDeviceSize alignment);
		void Reset();
		void Destroy(VKNDevice* pDevice);

		inline VkDeviceSize GetSizeLeft() const 
		{ 
			return m_SizeInBytes - m_Offset;
		}

		inline uint8* GetHostMemory() const
		{
			return m_Memory.pHostMemory;
		}

		inline VkBuffer GetVkBuffer() const
		{
			return m_VkBuffer;
		}
	private:
		void Init(VKNDevice* pDevice);
	private:
		VkBuffer m_VkBuffer;
		VKNAllocation m_Memory;
		VkDeviceSize m_SizeInBytes;
		VkDeviceSize m_Offset;
	};


	//------------------
	//VKNUploadAllocator
	//------------------

	class VKNUploadAllocator final
	{
	public:
		LAMBDA_NO_COPY(VKNUploadAllocator);

		VKNUploadAllocator(VKNDevice* pDevice, VkDeviceSize sizeInBytes = MB(1));
		~VKNUploadAllocator();

		VKNUploadAllocation Allocate(VkDeviceSize sizeInBytes, VkDeviceSize alignment);
		void Reset();
	private:
		VKNDevice* m_pDevice;
		VKNUploadPage* m_pCurrentPage;
		std::vector<VKNUploadPage*> m_DiscardedPages;		
	};
}