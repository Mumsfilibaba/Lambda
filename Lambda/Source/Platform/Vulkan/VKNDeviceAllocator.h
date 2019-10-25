#pragma once
#include "Graphics/Core/DeviceObjectBase.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VKNDevice;

	//-------------
	//VKNAllocation
	//-------------

	struct VKNAllocation
	{
        int32			PageID			   = -1;
        int32           BlockID            = -1;
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
        VKNMemoryBlock* pNext               = nullptr;
        VKNMemoryBlock* pPrevious           = nullptr;
        uint32          ID                  = 0;
        VkDeviceSize    SizeInBytes			= 0;
		VkDeviceSize    PaddedSizeInBytes	= 0;
        VkDeviceSize    DeviceMemoryOffset  = 0;
        bool            IsFree              = true;
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
		
		inline uint64 GetTotalReserved() const { return m_TotalReserved; }
		inline uint64 GetTotalAllocated() const { return m_TotalAllocated; }
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

	//-------------------------
	//VKNDescriptorSetAllocator
	//-------------------------

	class VKNDescriptorSetAllocator
	{
	public:
		LAMBDA_NO_COPY(VKNDescriptorSetAllocator);

		VKNDescriptorSetAllocator(VKNDevice* m_pDevice, uint32 uniformBufferCount, uint32 dynamicUniformBufferCount, uint32 samplerCount, uint32 sampledImageCount, uint32 combinedImageSamplerCount, uint32 numSets);
		~VKNDescriptorSetAllocator() = default;

		VkDescriptorSet Allocate(VkDescriptorSetLayout descriptorSetLayout);
		void Destroy(VkDevice device);
	private:
		void Init();
	private:
		VKNDevice* m_pDevice;
		VkDescriptorPool m_Pool;
		uint32 m_NumSets;
		uint32 m_UniformBufferCount;
		uint32 m_DynamicUniformBufferCount;
		uint32 m_SamplerCount;
		uint32 m_SampledImageCount;
		uint32 m_CombinedImageSamplerCount;
		uint32 m_SetCount;
	};
}
