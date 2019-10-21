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
        int32			ChunkID             = -1;
        int32           BlockID             = -1;
        uint8*          pHostMemory         = nullptr;
        VkDeviceSize    SizeInBytes         = 0;
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
        bool IsOnSamePage(VkDeviceSize aOffset, VkDeviceSize aSize, VkDeviceSize bOffset, VkDeviceSize pageSize);
        void Deallocate(VKNAllocation& allocation);
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

		bool Allocate(VKNAllocation& allocation, const VkMemoryRequirements& memoryRequirements, ResourceUsage usage);
		void Deallocate(VKNAllocation& allocation);
		void EmptyGarbageMemory();
		
		inline uint64 GetTotalReserved() const { return m_TotalReserved; }
		inline uint64 GetTotalAllocated() const { return m_TotalAllocated; }
	private:
		VKNDevice* m_pDevice;
        uint64 m_FrameIndex;
		std::vector<VKNMemoryPage*> m_Chunks;
		std::vector<std::vector<VKNAllocation>>	m_MemoryToDeallocate;
		uint64 m_TotalAllocated;
		uint64 m_TotalReserved;
		uint64 m_MaxAllocations;
        VkDeviceSize m_BufferImageGranularity;
	};

	//--------------------
	//VKNDynamicAllocation
	//--------------------

	struct VKNDynamicAllocation
	{
		int32			ChunkID = -1;
		int32           BlockID = -1;
		VkDeviceSize    SizeInBytes		= 0;
		VkDeviceSize    BufferOffset	= 0;
		uint8*			pHostMemory		= nullptr;
		VkBuffer		Buffer			= VK_NULL_HANDLE;
	};

	//---------------------
	//VKNDynamicMemoryBlock
	//---------------------

	struct VKNDynamicMemoryBlock
	{
		VKNDynamicMemoryBlock* pNext = nullptr;
		VKNDynamicMemoryBlock* pPrevious = nullptr;
		uint32          ID					= 0;
		VkDeviceSize    SizeInBytes			= 0;
		VkDeviceSize    PaddedSizeInBytes	= 0;
		VkDeviceSize    BufferOffset		= 0;
		bool            IsFree				= true;
	};

	//---------------------
	//VKNDynamicMemoryPage
	//---------------------

	class VKNDynamicMemoryPage
	{
	public:
		LAMBDA_NO_COPY(VKNDynamicMemoryPage);

		VKNDynamicMemoryPage(VKNDevice* pDevice, uint32 id, VkDeviceSize sizeInBytes);
		~VKNDynamicMemoryPage() = default;

		bool Allocate(VKNDynamicAllocation& allocation, VkDeviceSize sizeInBytes, VkDeviceSize alignment);
		void Deallocate(VKNDynamicAllocation& allocation);
		void Destroy(VKNDevice* pDevice);
	private:
		void Init(VKNDevice* pDevice);
	private:
		VKNDynamicMemoryBlock* m_pBlockHead;
		VkBuffer m_Buffer;
		VKNAllocation m_Memory;
		const uint32 m_ID;
		const uint64 m_SizeInBytes;
		uint32 m_BlockCount;
	};

	//-------------------------
	//VKNDynamicMemoryAllocator
	//-------------------------

	class VKNDynamicMemoryAllocator final
	{
	public:
		LAMBDA_NO_COPY(VKNDynamicMemoryAllocator);

		VKNDynamicMemoryAllocator(VKNDevice* pDevice);
		~VKNDynamicMemoryAllocator();

		bool Allocate(VKNDynamicAllocation& allocation, uint64 sizeInBytes, uint64 alignment);
		void Deallocate(VKNDynamicAllocation& allocation);
		void EmptyGarbageMemory();

		inline uint64 GetTotalReserved() const	{ return m_TotalReserved; }
		inline uint64 GetTotalAllocated() const { return m_TotalAllocated; }
	private:
		VKNDevice* m_pDevice;
		uint64 m_FrameIndex;
		std::vector<VKNDynamicMemoryPage*> m_Chunks;
		std::vector<std::vector<VKNDynamicAllocation>>	m_MemoryToDeallocate;
		uint64 m_TotalReserved;
		uint64 m_TotalAllocated;
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
