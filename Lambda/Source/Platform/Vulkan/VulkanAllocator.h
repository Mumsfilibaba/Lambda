#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Lambda
{
	//------------
	//VulkanMemory
	//------------

	struct VulkanMemory
	{
        int32			ChunkID             = -1;
        int32           BlockID             = -1;
        uint8*          pHostMemory         = nullptr;
        uint64          Size                = 0;
        uint64          DeviceMemoryOffset  = 0;
        VkDeviceMemory  DeviceMemory        = VK_NULL_HANDLE;
	};

	//----------------
	//IVulkanAllocator
	//----------------

	class IVulkanAllocator
	{
	public:
		LAMBDA_INTERFACE(IVulkanAllocator);

		IVulkanAllocator() = default;
		~IVulkanAllocator() = default;

		virtual bool Allocate(VulkanMemory* pAllocation, const VkMemoryRequirements& memoryRequirements, ResourceUsage usage) = 0;
		virtual void Deallocate(VulkanMemory* pAllocation) = 0;
		virtual void DefferedDeallocate(VulkanMemory* pAllocation, uint64 frameCount) = 0;
		virtual void CleanGarbageMemory(uint64 frameCount) = 0;
		virtual void Destroy(VkDevice device) = 0;
	};

    //-----------------
    //VulkanMemoryBlock
    //-----------------
    
    struct VulkanMemoryBlock
    {
        VulkanMemoryBlock*  pNext               = nullptr;
        VulkanMemoryBlock*  pPrevious           = nullptr;
        uint32              ID                  = 0;
        uint32              Size                = 0;
        uint32              DeviceMemoryOffset  = 0;
        bool                IsFree              = true;
    };
    
	//-----------------
	//VulkanMemoryChunk
	//-----------------

	class VulkanMemoryChunk
	{
	public:
		LAMBDA_NO_COPY(VulkanMemoryChunk);

		VulkanMemoryChunk(uint32 id, uint64 sizeInBytes, uint64 memoryType, ResourceUsage usage);
		~VulkanMemoryChunk() = default;

		bool    Allocate(VulkanMemory* pAllocation, uint64 sizeInBytes, uint64 alignment);
		void    Deallocate(VulkanMemory* pAllocation);
		void	Destroy(VkDevice device);
		uint32  GetMemoryType() const;

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
		VulkanMemoryBlock*  m_pBlockHead;
		bool				m_IsMapped;
	};

	//---------------------
	//VulkanDeviceAllocator
	//---------------------

	class VulkanDeviceAllocator final : public IVulkanAllocator
	{
	public:
		LAMBDA_NO_COPY(VulkanDeviceAllocator);

		VulkanDeviceAllocator();
		~VulkanDeviceAllocator() = default;

		virtual bool Allocate(VulkanMemory* pAllocation, const VkMemoryRequirements& memoryRequirements, ResourceUsage usage) override final;
		virtual void Deallocate(VulkanMemory* pAllocation) override final;
		virtual void DefferedDeallocate(VulkanMemory* pAllocation, uint64 frameCount) override final;
		virtual void CleanGarbageMemory(uint64 frameCount) override final;
		virtual void Destroy(VkDevice device) override final;

	private:
		uint64									m_MaxAllocations;
		std::vector<VulkanMemoryChunk*>			m_Chunks;
		std::vector<std::vector<VulkanMemory>>	m_MemoryToDeallocate;
	};
}
