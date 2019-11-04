#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include "../Vulkan.h"

namespace Lambda
{
	class VKNDevice;
	class VKNMemoryPage;
	struct VKNMemoryBlock;

	//-------------
	//VKNAllocation
	//-------------

	struct VKNAllocation
	{
		VKNMemoryBlock* pBlock			   = nullptr;
        uint8*          pHostMemory        = nullptr;
        VkDeviceSize    SizeInBytes        = 0;
        VkDeviceSize    DeviceMemoryOffset = 0;
        VkDeviceMemory  DeviceMemory       = VK_NULL_HANDLE;
	};

    //--------------
    //VKNMemoryBlock
    //--------------
    
    struct VKNMemoryBlock
    {
		VKNMemoryPage*	pPage				= nullptr;
        VKNMemoryBlock* pNext               = nullptr;
        VKNMemoryBlock* pPrevious           = nullptr;
        VkDeviceSize    SizeInBytes			= 0;
		VkDeviceSize    PaddedSizeInBytes	= 0;
        VkDeviceSize    DeviceMemoryOffset  = 0;
		uint32          ID                  = 0;
        bool            IsFree              = true;
    };
    
	//--------------
	//VKNMemoryPage
	//--------------

	class VKNMemoryPage
	{
	public:
		LAMBDA_NO_COPY(VKNMemoryPage);

		VKNMemoryPage(VKNDevice* pVkDevice, uint32 id, VkDeviceSize sizeInBytes, uint32 memoryType, VkMemoryPropertyFlags properties);
		~VKNMemoryPage() = default;

		bool Allocate(VKNAllocation& allocation, VkDeviceSize sizeInBytes, VkDeviceSize alignment, VkDeviceSize granularity);
		void Deallocate(VKNAllocation& allocation);
		void Destroy(VKNDevice* pVkDevice);


		_forceinline bool IsEmpty() const
		{
			return m_pHead->pPrevious == nullptr && m_pHead->pNext == nullptr && m_pHead->IsFree;
		}


		_forceinline uint64 GetSize() const
		{
			return m_SizeInBytes;
		}


		_forceinline uint32 GetMemoryType() const 
		{ 
			return m_MemoryType; 
		}
	private:
        bool IsOnSamePage(VkDeviceSize aOffset, VkDeviceSize aSize, VkDeviceSize bOffset, VkDeviceSize pageSize);
		void Init(VKNDevice* pVkDevice);
		void Map(VKNDevice* pVkDevice);
		void Unmap(VKNDevice* pVkDevice);
	private:
		VKNMemoryBlock*	m_pHead;
		uint8* m_pHostMemory;
		VkDeviceMemory	m_VkDeviceMemory;
		const uint64 m_SizeInBytes;
		VkMemoryPropertyFlags m_Properties;
		const uint32 m_ID;
		const uint32 m_MemoryType;
        uint32 m_BlockCount;
		bool m_IsMapped;
	};

	//------------------
	//VKNDeviceAllocator
	//------------------

	class VKNDeviceAllocator final
	{
	public:
		LAMBDA_NO_COPY(VKNDeviceAllocator);

		VKNDeviceAllocator(VKNDevice* pDevice);
		~VKNDeviceAllocator();

		bool Allocate(VKNAllocation& allocation, const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags properties);
		void Deallocate(VKNAllocation& allocation);
		void EmptyGarbageMemory();
		

		_forceinline uint64 GetTotalReserved() const 
		{ 
			return m_TotalReserved; 
		}
		
		
		_forceinline uint64 GetTotalAllocated() const 
		{ 
			return m_TotalAllocated; 
		}
	private:
		std::vector<std::vector<VKNAllocation>>	m_MemoryToDeallocate;
		std::vector<VKNMemoryPage*> m_Pages;
		VKNDevice* m_pVkDevice;
        uint64 m_FrameIndex;
		uint64 m_TotalAllocated;
		uint64 m_TotalReserved;
		uint64 m_MaxAllocations;
        VkDeviceSize m_BufferImageGranularity;
	};
}
