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
        uint64          Size                = 0;
        uint64          DeviceMemoryOffset  = 0;
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

		virtual bool Allocate(VKNMemory* pAllocation, const VkMemoryRequirements& memoryRequirements, ResourceUsage usage) = 0;
		virtual void Deallocate(VKNMemory* pAllocation) = 0;
		virtual void DefferedDeallocate(VKNMemory* pAllocation, uint64 frameCount) = 0;
		virtual void CleanGarbageMemory(uint64 frameCount) = 0;
		virtual void Destroy(VkDevice device) = 0;
	};

    //--------------
    //VKNMemoryBlock
    //--------------
    
    struct VKNMemoryBlock
    {
        VKNMemoryBlock* pNext               = nullptr;
        VKNMemoryBlock* pPrevious           = nullptr;
        uint32          ID                  = 0;
        uint64          Size                = 0;
        uint64          DeviceMemoryOffset  = 0;
        bool            IsFree              = true;
    };
    
	//--------------
	//VKNMemoryChunk
	//--------------

	class VKNMemoryChunk
	{
	public:
		LAMBDA_NO_COPY(VKNMemoryChunk);

		VKNMemoryChunk(uint32 id, uint64 sizeInBytes, uint32 memoryType, ResourceUsage usage);
		~VKNMemoryChunk() = default;

		bool    Allocate(VKNMemory* pAllocation, uint64 sizeInBytes, uint64 alignment);
		void    Deallocate(VKNMemory* pAllocation);
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

		virtual bool Allocate(VKNMemory* pAllocation, const VkMemoryRequirements& memoryRequirements, ResourceUsage usage) override final;
		virtual void Deallocate(VKNMemory* pAllocation) override final;
		virtual void DefferedDeallocate(VKNMemory* pAllocation, uint64 frameCount) override final;
		virtual void CleanGarbageMemory(uint64 frameCount) override final;
		virtual void Destroy(VkDevice device) override final;

	private:
		uint64								m_MaxAllocations;
		std::vector<VKNMemoryChunk*>		m_Chunks;
		std::vector<std::vector<VKNMemory>>	m_MemoryToDeallocate;
	};
}
