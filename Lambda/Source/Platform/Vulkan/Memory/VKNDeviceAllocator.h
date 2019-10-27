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
        bool            IsFree              = true;
		uint32          ID                  = 0;
    };
    
	//--------------
	//VKNMemoryPage
	//--------------

	class VKNMemoryPage
	{
	public:
		LAMBDA_NO_COPY(VKNMemoryPage);

		VKNMemoryPage(VKNDevice* pDevice, uint32 id, VkDeviceSize sizeInBytes, uint32 memoryType, ResourceUsage usage);
		~VKNMemoryPage() = default;

		bool Allocate(VKNAllocation& allocation, VkDeviceSize sizeInBytes, VkDeviceSize alignment, VkDeviceSize granularity);
		void Deallocate(VKNAllocation& allocation);
		void Destroy(VKNDevice* pDevice);


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
		void Init(VKNDevice* pDevice);
		void Map(VKNDevice* pDevice);
		void Unmap(VKNDevice* pDevice);
	private:
		VkDeviceMemory	m_DeviceMemory;
		VKNMemoryBlock*	m_pHead;
		uint8* m_pHostMemory;
		const ResourceUsage	m_Usage;
		const uint32 m_ID;
		const uint32 m_MemoryType;
		const uint64 m_SizeInBytes;
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

		bool Allocate(VKNAllocation& allocation, const VkMemoryRequirements& memoryRequirements, ResourceUsage usage);
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
		VKNDevice* m_pDevice;
        uint64 m_FrameIndex;
		std::vector<VKNMemoryPage*> m_Pages;
		std::vector<std::vector<VKNAllocation>>	m_MemoryToDeallocate;
		uint64 m_TotalAllocated;
		uint64 m_TotalReserved;
		uint64 m_MaxAllocations;
        VkDeviceSize m_BufferImageGranularity;
	};
}
