#pragma once
#include "Graphics/Core/DeviceObjectBase.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VKNDevice;

	//---------
	//VKNMemory
	//---------

	struct VKNMemory
	{
        int32			ChunkID             = -1;
        int32           BlockID             = -1;
        uint8*          pHostMemory         = nullptr;
        VkDeviceSize    Size                = 0;
        VkDeviceSize    DeviceMemoryOffset  = 0;
        VkDeviceMemory  DeviceMemory        = VK_NULL_HANDLE;
	};

    //--------------
    //VKNMemoryBlock
    //--------------
    
    struct VKNMemoryBlock
    {
        VKNMemoryBlock* pNext               = nullptr;
        VKNMemoryBlock* pPrevious           = nullptr;
        uint32          ID                  = 0;
        VkDeviceSize    Size                = 0;
        VkDeviceSize    DeviceMemoryOffset  = 0;
        bool            IsFree              = true;
    };
    
	//--------------
	//VKNMemoryChunk
	//--------------

	class VKNMemoryChunk
	{
	public:
		LAMBDA_NO_COPY(VKNMemoryChunk);

		VKNMemoryChunk(VKNDevice* pDevice, uint32 id, VkDeviceSize sizeInBytes, uint32 memoryType, ResourceUsage usage);
		~VKNMemoryChunk() = default;

		bool Allocate(VKNMemory& allocation, VkDeviceSize sizeInBytes, VkDeviceSize alignment, VkDeviceSize granularity);
        bool IsOnSamePage(VkDeviceSize aOffset, VkDeviceSize aSize, VkDeviceSize bOffset, VkDeviceSize pageSize);
        void Deallocate(VKNMemory& allocation);
		void Destroy(VKNDevice* pDevice);
		inline uint32 GetMemoryType() const { return m_MemoryType; }
	private:
		void Init(VKNDevice* pDevice);
		void Map(VKNDevice* pDevice);
		void Unmap(VKNDevice* pDevice);
	private:
		uint8*				m_pHostMemory;
		VkDeviceMemory		m_DeviceMemory;
		const ResourceUsage	m_Usage;
		const uint32		m_ID;
		const uint32		m_MemoryType;
		const uint64		m_SizeInBytes;
        uint32              m_BlockCount;
		VKNMemoryBlock*		m_pBlockHead;
		bool				m_IsMapped;
	};

	//------------
	//VKNAllocator
	//------------

	class VKNAllocator final
	{
	public:
		LAMBDA_NO_COPY(VKNAllocator);

		VKNAllocator(VKNDevice* pDevice);
		~VKNAllocator();

		virtual bool Allocate(VKNMemory& allocation, const VkMemoryRequirements& memoryRequirements, ResourceUsage usage);
		virtual void Deallocate(VKNMemory& allocation);
		virtual void EmptyGarbageMemory();
		virtual uint64 GetTotalReserved() const;
		virtual uint64 GetTotalAllocated() const;
	private:
		VKNDevice* m_pDevice;
		uint64 m_MaxAllocations;
		uint64 m_TotalReserved;
		uint64 m_TotalAllocated;
        uint64 m_FrameIndex;
        VkDeviceSize m_BufferImageGranularity;
		std::vector<VKNMemoryChunk*> m_Chunks;
		std::vector<std::vector<VKNMemory>>	m_MemoryToDeallocate;
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
