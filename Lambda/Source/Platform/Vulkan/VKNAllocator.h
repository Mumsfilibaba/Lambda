#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Lambda
{
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

	//-------------
	//IVKNAllocator
	//-------------

	class IVKNAllocator
	{
	public:
		LAMBDA_INTERFACE(IVKNAllocator);

		IVKNAllocator() = default;
		~IVKNAllocator() = default;

		virtual bool Allocate(VKNMemory& allocation, const VkMemoryRequirements& memoryRequirements, ResourceUsage usage) = 0;
		virtual void Deallocate(VKNMemory& allocation) = 0;
		virtual void EmptyGarbageMemory() = 0;
		virtual void Destroy(VkDevice device) = 0;
		virtual uint64 GetTotalReserved() const = 0;
		virtual uint64 GetTotalAllocated() const = 0;
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

		VKNMemoryChunk(uint32 id, VkDeviceSize sizeInBytes, uint32 memoryType, ResourceUsage usage);
		~VKNMemoryChunk() = default;

		bool Allocate(VKNMemory& allocation, VkDeviceSize sizeInBytes, VkDeviceSize alignment, VkDeviceSize granularity);
        bool IsOnSamePage(VkDeviceSize aOffset, VkDeviceSize aSize, VkDeviceSize bOffset, VkDeviceSize pageSize);
        void Deallocate(VKNMemory& allocation);
		void Destroy(VkDevice device);
		uint32 GetMemoryType() const;
	private:
		void Init();
		void Map();
		void Unmap();
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

	class VKNAllocator final : public IVKNAllocator
	{
	public:
		LAMBDA_NO_COPY(VKNAllocator);

		VKNAllocator();
		~VKNAllocator() = default;

		virtual bool Allocate(VKNMemory& allocation, const VkMemoryRequirements& memoryRequirements, ResourceUsage usage) override final;
		virtual void Deallocate(VKNMemory& allocation) override final;
		virtual void EmptyGarbageMemory() override final;
		virtual void Destroy(VkDevice device) override final;
		virtual uint64 GetTotalReserved() const override final;
		virtual uint64 GetTotalAllocated() const override final;
	private:
		uint64								m_MaxAllocations;
		uint64								m_TotalReserved;
		uint64								m_TotalAllocated;
        uint64                              m_FrameIndex;
        VkDeviceSize                        m_BufferImageGranularity;
		std::vector<VKNMemoryChunk*>		m_Chunks;
		std::vector<std::vector<VKNMemory>>	m_MemoryToDeallocate;
	};

	//-------------------------
	//VKNDescriptorSetAllocator
	//-------------------------

	class VKNDescriptorSetAllocator
	{
	public:
		LAMBDA_NO_COPY(VKNDescriptorSetAllocator);

		VKNDescriptorSetAllocator(uint32 uniformBufferCount, uint32 dynamicUniformBufferCount, uint32 samplerCount, uint32 sampledImageCount, uint32 combinedImageSamplerCount, uint32 numSets);
		~VKNDescriptorSetAllocator() = default;

		VkDescriptorSet Allocate(VkDescriptorSetLayout descriptorSetLayout);
		void Destroy(VkDevice device);
	private:
		void Init();
	private:
		VkDescriptorPool				m_Pool;
		uint32							m_NumSets;
		uint32							m_UniformBufferCount;
		uint32							m_DynamicUniformBufferCount;
		uint32							m_SamplerCount;
		uint32							m_SampledImageCount;
		uint32							m_CombinedImageSamplerCount;
		uint32							m_SetCount;
	};

	//------------------------
	//VKNDescriptorPoolManager
	//------------------------

	class VKNDescriptorPoolManager
	{
	public:
		LAMBDA_NO_COPY(VKNDescriptorPoolManager);

		VKNDescriptorPoolManager();
		~VKNDescriptorPoolManager() = default;

		void DeallocatePool(VkDescriptorPool pool);
		void Cleanup();
	private:
		uint32										m_FrameCount;
		uint32										m_CurrentFrame;
		std::vector<std::vector<VkDescriptorPool>>	m_OldPools;
	private:
		static VKNDescriptorPoolManager* s_pInstance;
	public:
		static VKNDescriptorPoolManager& GetInstance();
	};
}
