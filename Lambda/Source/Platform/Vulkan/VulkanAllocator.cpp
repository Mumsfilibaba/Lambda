#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanAllocator.h"
#include "VulkanUtilities.h"

namespace Lambda
{
	//-----------------
	//VulkanMemoryChunk
	//-----------------

	VulkanMemoryChunk::VulkanMemoryChunk(uint32 id, uint64 sizeInBytes, uint64 memoryType, ResourceUsage usage)
		: m_Usage(usage),
		m_ID(id),
		m_MemoryType(memoryType),
		m_SizeInBytes(sizeInBytes),
        m_BlockCount(0),
		m_pBlockHead(nullptr),
		m_pHostMemory(nullptr),
		m_IsMapped(false)
	{
		Init();
	}


	void VulkanMemoryChunk::Init()
	{
		//Allocate device memory
		VkMemoryAllocateInfo allocInfo  = {};
		allocInfo.sType				    = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext				    = nullptr;
		allocInfo.allocationSize	    = m_SizeInBytes;
		allocInfo.memoryTypeIndex	    = m_MemoryType;

		VulkanGraphicsDevice& device = VulkanGraphicsDevice::GetInstance();
		if (vkAllocateMemory(device.GetDevice(), &allocInfo, nullptr, &m_DeviceMemory) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate MemoryChunk\n");
			return;
		}
		else
		{
			LOG_DEBUG_WARNING("Vulkan: Allocated '%d' bytes for MemoryChunk\n", allocInfo.allocationSize);
		}

		//Setup first block
        m_pBlockHead = DBG_NEW VulkanMemoryBlock();
        m_pBlockHead->pNext					= nullptr;
        m_pBlockHead->pPrevious				= nullptr;
        m_pBlockHead->IsFree				= true;
        m_pBlockHead->ID					= m_BlockCount++;
		m_pBlockHead->Size					= m_SizeInBytes;
		m_pBlockHead->DeviceMemoryOffset    = 0;
		
		//If this is CPU visible -> Map
		if (m_Usage == RESOURCE_USAGE_DYNAMIC)
		{
			Map();
		}
	}


	bool VulkanMemoryChunk::Allocate(VulkanMemory* pAllocation, uint64 sizeInBytes, uint64 alignment)
	{
        LAMBDA_ASSERT(pAllocation != nullptr);
        
        VkDeviceSize offset         = 0;
        VkDeviceSize padding        = 0;
        VulkanMemoryBlock* pBestFit = nullptr;
        
        //Find enough free space, and find the block that best fits
        for (VulkanMemoryBlock* pCurrent = m_pBlockHead; pCurrent != nullptr; pCurrent = pCurrent->pNext)
        {
            //Check if the block is allocated or not
            if (!pCurrent->IsFree)
                continue;
            
            //Align the offset
            offset  = Math::AlignUp<uint64>(pCurrent->DeviceMemoryOffset, alignment);
            padding = offset - pCurrent->DeviceMemoryOffset;
            
            if (pCurrent->Size <= padding + sizeInBytes)
                continue;
            
            //Set bestfit
            if (pBestFit)
            {
                if (pBestFit->Size <= pCurrent->Size)
                    continue;
            }
            
            pBestFit = pCurrent;
        }
        

        //Did we find a suitable block to make the allocation?
        if (!pBestFit)
        {
            return false;
		}

        
        //Create a block for the padding (Allocating a texture in a block can result in a padding of 256 bytes which is enough for a buffer)
        if (padding > 0)
        {
			//Save the old block (Will be padding)
            VulkanMemoryBlock* pPadding		= pBestFit;

			//Create a new block that fill house the allocation
			pBestFit						= DBG_NEW VulkanMemoryBlock();
			pBestFit->ID                    = m_BlockCount++;
			pBestFit->DeviceMemoryOffset    = offset;
			pBestFit->Size                  = pPadding->Size - padding;
			pBestFit->IsFree                = true;
            
			//Resize padding block
			pPadding->Size = padding;

			//Set pointers
			pBestFit->pNext		= pPadding->pNext;
			pBestFit->pPrevious = pPadding;

            pPadding->pNext     = pBestFit;
        }
        
        
        if (pBestFit->Size > sizeInBytes)
        {
            //Create a new block after allocation
            VulkanMemoryBlock* pBlock = DBG_NEW VulkanMemoryBlock();
            pBlock->ID                  = m_BlockCount++;
            pBlock->Size                = pBestFit->Size - sizeInBytes;
            pBlock->DeviceMemoryOffset  = pBestFit->DeviceMemoryOffset + sizeInBytes;
            pBlock->IsFree              = true;
            
			//Set pointers
            pBlock->pNext       = pBestFit->pNext;
            pBlock->pPrevious   = pBestFit;
            
            pBestFit->Size      = sizeInBytes;
            pBestFit->pNext     = pBlock;
        }

        
        //Setup allocation
        pAllocation->BlockID            = pBestFit->ID;
        pAllocation->ChunkID            = m_ID;
        pAllocation->DeviceMemory       = m_DeviceMemory;
        pAllocation->DeviceMemoryOffset = pBestFit->DeviceMemoryOffset;
        pAllocation->Size               = pBestFit->Size;
        if (m_Usage == RESOURCE_USAGE_DYNAMIC)
        {
            pAllocation->pHostMemory = m_pHostMemory + pAllocation->DeviceMemoryOffset;
        }
        else
        {
            pAllocation->pHostMemory = nullptr;
        }

        
        //Set bestfit to be used
        pBestFit->IsFree = false;
        
        
        VulkanMemoryBlock* pCurrent = m_pBlockHead;
        LOG_DEBUG_WARNING("Allocated blocks in chunk %u:\n", m_ID);
        while (pCurrent)
        {
            LOG_DEBUG_WARNING("    VulkanBlock: ID=%u, Offset=%u, Size=%u, IsFree=%s\n", pCurrent->ID, pCurrent->DeviceMemoryOffset, pCurrent->Size, pCurrent->IsFree ? "True" : "False");
            pCurrent = pCurrent->pNext;
        }
        
		return true;
	}


	void VulkanMemoryChunk::Map()
	{
		//If not mapped -> map
		if (!m_IsMapped)
		{
			VulkanGraphicsDevice& device = VulkanGraphicsDevice::GetInstance();

			void* pMemory = nullptr;
			vkMapMemory(device.GetDevice(), m_DeviceMemory, 0, VK_WHOLE_SIZE, 0, &pMemory);

			m_pHostMemory   = reinterpret_cast<uint8*>(pMemory);
			m_IsMapped      = true;
		}
	}


	void VulkanMemoryChunk::Unmap()
	{
		//If mapped -> unmap
		if (m_IsMapped)
		{
			VulkanGraphicsDevice& device = VulkanGraphicsDevice::GetInstance();
			vkUnmapMemory(device.GetDevice(), m_DeviceMemory);

			m_pHostMemory   = nullptr;
			m_IsMapped      = false;
		}
	}


	void VulkanMemoryChunk::Deallocate(VulkanMemory* pAllocation)
	{
        /*VulkanMemoryBlock* pCurrent = m_pBlockHead;
        while (!pCurrent)
        {
            VulkanMemory& current = pCurrent->Memory;
            if (current.Offset + current.Size == allocation.Offset)
            {
                current.Size = current.Size + allocation.Size;
                if (pCurrent->pNext)
                {
                    VulkanMemoryBlock* pNext = pCurrent->pNext;
                    VulkanMemory& next = pNext->Memory;
                    
                    if (next.Offset == allocation.Offset + allocation.Size)
                    {
                        // |current|allocation|next|
                        // |-------|          |----|   |---|
                        // |-------|??????????|----|   |---|
                        // |-----------------------|   |---|
                        
                        // Remove the next block since pCurrent now stretes over it
                        current.Size = current.Size + allocation.Size
                        
                    }
                }
            }
        }
        
        
		//Search through the blocks and see if they come after or before eachother
        for (auto block = m_Blocks.begin(); block != m_Blocks.end(); block += 2)
        {
            if (block->Offset + block->Size == allocation.Offset)
            {
                block->Size += allocation.Size;
                return;
            }
            else if (allocation.Offset + allocation.Size == block->Offset)
            {
                block->Size        += allocation.Size;
                block->Offset    -= allocation.Size;
                return;
            }
        }

        //Otherwise just push block
        m_Blocks.push_back(allocation);*/
	}


	void VulkanMemoryChunk::Destroy(VkDevice device)
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

		if (m_DeviceMemory != VK_NULL_HANDLE)
		{
			Unmap();

			vkFreeMemory(device, m_DeviceMemory, nullptr);
			m_DeviceMemory = VK_NULL_HANDLE;

			LOG_SYSTEM(LOG_SEVERITY_WARNING, "Vulkan: Deallocated MemoryChunk\n");
		}

		delete this;
	}


	uint32 VulkanMemoryChunk::GetMemoryType() const
	{
		return m_MemoryType;
	}


	//---------------------
	//VulkanDeviceAllocator
	//---------------------


	VulkanDeviceAllocator::VulkanDeviceAllocator()
		: m_MaxAllocations(0),
		m_Chunks(),
		m_MemoryToDeallocate()
	{
		VulkanGraphicsDevice& vkDevice = VulkanGraphicsDevice::GetInstance();

		uint32 frameCount = vkDevice.GetDeviceSettings().FramesAhead;
		m_MemoryToDeallocate.resize(frameCount);

		VkPhysicalDeviceProperties properties = vkDevice.GetPhysicalDeviceProperties();
		m_MaxAllocations = properties.limits.maxMemoryAllocationCount;
	}


	bool VulkanDeviceAllocator::Allocate(VulkanMemory* pAllocation, const VkMemoryRequirements& memoryRequirements, ResourceUsage usage)
	{
		LAMBDA_ASSERT_PRINT(memoryRequirements.size < MB(256), "Allocations must be smaller than 256MB\n");

		
		//Set memoryproperty based on resource usage
		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		if (usage == RESOURCE_USAGE_DYNAMIC)
		{
			properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}

		//Get needed memorytype
		VulkanGraphicsDevice& device = VulkanGraphicsDevice::GetInstance();
		uint32 memoryType = FindMemoryType(device.GetPhysicalDevice(), memoryRequirements.memoryTypeBits, properties);

		//Try allocating from existing chunk
		for (auto chunk : m_Chunks)
		{
			if (chunk->GetMemoryType() == memoryType)
			{
				if (chunk->Allocate(pAllocation, memoryRequirements.size, memoryRequirements.alignment))
				{
					return true;
				}
			}
		}

        
		LAMBDA_ASSERT_PRINT(m_Chunks.size() < m_MaxAllocations, "Max number of allocations already reached\n");

        
		//Allocate new chunk
		VulkanMemoryChunk* pChunk = DBG_NEW VulkanMemoryChunk(uint32(m_Chunks.size()), MB(256), memoryType, usage);
		m_Chunks.emplace_back(pChunk);
		
        
		LOG_SYSTEM(LOG_SEVERITY_WARNING, " [DEVICE ALLOCATOR] Allocated Memory chunk. Number of allocations: '%llu'. Max allocations: '%llu'. Memory type: %d, \n", m_Chunks.size(), m_MaxAllocations, memoryType);

        
		return m_Chunks.back()->Allocate(pAllocation, memoryRequirements.size, memoryRequirements.alignment);
	}


	void VulkanDeviceAllocator::Deallocate(VulkanMemory* pAllocation)
	{
        if (size_t(pAllocation->ChunkID) < m_Chunks.size() && pAllocation->DeviceMemory != VK_NULL_HANDLE)
        {
            VulkanMemoryChunk* pChunk = m_Chunks[pAllocation->ChunkID];
            pChunk->Deallocate(pAllocation);
        }
	}

	
	void VulkanDeviceAllocator::DefferedDeallocate(VulkanMemory* pAllocation, uint64 frameCount)
	{
        if (size_t(pAllocation->ChunkID) < m_Chunks.size() && pAllocation->DeviceMemory != VK_NULL_HANDLE)
        {
            //m_MemoryToDeallocate[frameCount].emplace_back(pAllocation);
        }
	}


	void VulkanDeviceAllocator::CleanGarbageMemory(uint64 frameCount)
	{
		auto& memoryBlocks = m_MemoryToDeallocate[frameCount];
		if (memoryBlocks.size() > 0)
		{
			for (auto& memory : memoryBlocks)
			{
				//Deallocate(memory);
			}

			memoryBlocks.clear();
		}
	}


	void VulkanDeviceAllocator::Destroy(VkDevice device)
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

		LOG_SYSTEM(LOG_SEVERITY_WARNING, "Vulkan: Deleting DeviceAllocator. Number of chunks: %u\n", m_Chunks.size());
		for (auto chunk : m_Chunks)
		{
			if (chunk)
			{
				chunk->Destroy(device);
			}
		}

		delete this;
	}
}
