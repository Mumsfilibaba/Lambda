#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VKNDeviceAllocator.h"
#include "../VKNDevice.h"
#include "../VKNUtilities.h"

//#define LAMBDA_ALLOCATOR_DEBUG
#define LAMBDA_ALLOCATOR_MAX MB(16)

namespace Lambda
{
	//--------------
	//VKNMemoryPage
	//--------------

	constexpr float mb = 1024.0f * 1024.0f;

	VKNMemoryPage::VKNMemoryPage(VKNDevice* pDevice, uint32 id, VkDeviceSize sizeInBytes, uint32 memoryType, VkMemoryPropertyFlags properties)
		: m_Properties(properties),
		m_ID(id),
		m_MemoryType(memoryType),
		m_SizeInBytes(sizeInBytes),
        m_BlockCount(0),
		m_pHead(nullptr),
		m_pHostMemory(nullptr),
		m_IsMapped(false)
	{
		Init(pDevice);
	}


	void VKNMemoryPage::Init(VKNDevice* pDevice)
	{
		//Allocate device memory
		VkMemoryAllocateInfo allocInfo  = {};
		allocInfo.sType				    = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext				    = nullptr;
		allocInfo.allocationSize	    = m_SizeInBytes;
		allocInfo.memoryTypeIndex	    = m_MemoryType;
		if (vkAllocateMemory(pDevice->GetVkDevice(), &allocInfo, nullptr, &m_DeviceMemory) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate MemoryPage\n");
			return;
		}
		else
		{
			LOG_DEBUG_WARNING("Vulkan: Allocated '%d' bytes for MemoryPage\n", allocInfo.allocationSize);
		}

		//Setup first block
        m_pHead = DBG_NEW VKNMemoryBlock();
		m_pHead->pPage					= this;
        m_pHead->pNext					= nullptr;
        m_pHead->pPrevious				= nullptr;
        m_pHead->IsFree					= true;
        m_pHead->ID						= m_BlockCount++;
		m_pHead->SizeInBytes			= m_SizeInBytes;
		m_pHead->PaddedSizeInBytes		= m_SizeInBytes;
		m_pHead->DeviceMemoryOffset		= 0;
		
		//If this is CPU visible -> Map
		if (m_Properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
			Map(pDevice);
		}
	}


	bool VKNMemoryPage::Allocate(VKNAllocation& allocation, VkDeviceSize sizeInBytes, VkDeviceSize alignment, VkDeviceSize granularity)
	{
        VkDeviceSize paddedDeviceOffset = 0;
        VkDeviceSize paddedSizeInBytes  = 0;
        VKNMemoryBlock* pBestFit = nullptr;
        
        //Find enough free space, and find the block that best fits
        for (VKNMemoryBlock* pCurrent = m_pHead; pCurrent != nullptr; pCurrent = pCurrent->pNext)
        {
            //Check if the block is allocated or not
            if (!pCurrent->IsFree)
                continue;
            
            //Does it fit into the block
            if (sizeInBytes > pCurrent->SizeInBytes)
                continue;
            
            //Align the offset
            paddedDeviceOffset = Math::AlignUp<uint64>(pCurrent->DeviceMemoryOffset, alignment);
            
            //Take granularity into account
            if (pCurrent->pPrevious != nullptr && granularity > 1)
            {
                VKNMemoryBlock* pPrevious = pCurrent->pPrevious;
                if (IsOnSamePage(pPrevious->DeviceMemoryOffset, pPrevious->SizeInBytes, paddedDeviceOffset, granularity))
                {
                    paddedDeviceOffset = Math::AlignUp(paddedDeviceOffset, granularity);
                }
            }
            
            //Calculate padding
            paddedSizeInBytes = sizeInBytes + (paddedDeviceOffset - pCurrent->DeviceMemoryOffset);
            
            //Does it still fit
            if (paddedSizeInBytes > pCurrent->SizeInBytes)
                continue;
            
            //Avoid granularity conflict
            if (granularity > 1 && pCurrent->pNext != nullptr)
            {
                VKNMemoryBlock* pNext = pCurrent->pNext;
                if (IsOnSamePage(paddedDeviceOffset, sizeInBytes, pNext->DeviceMemoryOffset, granularity))
                {
                    continue;
                }
            }
            
            pBestFit = pCurrent;
			break;
        }

        //Did we find a suitable block to make the allocation?
		if (pBestFit == nullptr)
		{
            return false;       
		}
        

        //        Free block
        //|--------------------------|
        //padding Allocation Remaining
        //|------|----------|--------|
        if (pBestFit->SizeInBytes > paddedSizeInBytes)
        {
            //Create a new block after allocation
            VKNMemoryBlock* pBlock = DBG_NEW VKNMemoryBlock();
			pBlock->pPage				= this;
			pBlock->ID                  = m_BlockCount++;
            pBlock->SizeInBytes			= pBestFit->SizeInBytes - paddedSizeInBytes;
			pBlock->PaddedSizeInBytes	= pBlock->SizeInBytes;
            pBlock->DeviceMemoryOffset  = pBestFit->DeviceMemoryOffset + paddedSizeInBytes;
            pBlock->IsFree              = true;
            
            //Set pointers
            pBlock->pNext     = pBestFit->pNext;
            pBlock->pPrevious = pBestFit;
            if (pBestFit->pNext)
                pBestFit->pNext->pPrevious = pBlock;
            pBestFit->pNext = pBlock;
        }
        
        //Update bestfit
        pBestFit->SizeInBytes		= paddedSizeInBytes;
		pBestFit->PaddedSizeInBytes = paddedSizeInBytes;
        pBestFit->IsFree			= false;
        
        //Setup allocation
		allocation.pBlock			  = pBestFit;
        allocation.DeviceMemory       = m_DeviceMemory;
        allocation.DeviceMemoryOffset = paddedDeviceOffset;
        allocation.SizeInBytes		  = sizeInBytes;
        if (m_Properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
            allocation.pHostMemory = m_pHostMemory + allocation.DeviceMemoryOffset;
        else
            allocation.pHostMemory = nullptr;

#if defined (LAMBDA_ALLOCATOR_DEBUG)
		{
			LOG_DEBUG_INFO("Vulkan: Memory Page '%d'\n", m_ID);
			for (VKNMemoryBlock* pCurrent = m_pHead; pCurrent != nullptr; pCurrent = pCurrent->pNext)
			{
				LOG_DEBUG_INFO("----Block %d----\n", pCurrent->ID);
				LOG_DEBUG_INFO("Starts at: %llu\n", pCurrent->DeviceMemoryOffset);
				LOG_DEBUG_INFO("Free: %s\n", pCurrent->IsFree ? "true" : "false");
				if (pCurrent->pPrevious)
				{
					VKNMemoryBlock* pPrevious = pCurrent->pPrevious;
					if ((pPrevious->DeviceMemoryOffset + pPrevious->PaddedSizeInBytes) > pCurrent->DeviceMemoryOffset)
					{
						LOG_DEBUG_WARNING("Overlapping memory in page '%d' between blocks '%d' and '%d'\n", m_ID, pPrevious->ID, pCurrent->ID);
					}
				}
				LOG_DEBUG_INFO("   End at: %llu\n", pCurrent->DeviceMemoryOffset + pCurrent->PaddedSizeInBytes);
				LOG_DEBUG_INFO("----------------\n", pCurrent->ID);
			}
		}
#endif

		return true;
	}


    bool VKNMemoryPage::IsOnSamePage(VkDeviceSize aOffset, VkDeviceSize aSize, VkDeviceSize bOffset, VkDeviceSize pageSize)
    {
        LAMBDA_ASSERT( aOffset + aSize <= bOffset && aSize > 0 && pageSize > 0 );
        
        VkDeviceSize aEnd       = aOffset + (aSize-1);
        VkDeviceSize aEndPage   = aEnd & ~(pageSize-1);
        VkDeviceSize bStart     = bOffset;
        VkDeviceSize bStartPage = bStart & ~(pageSize-1);
        return aEndPage == bStartPage;
    }


	void VKNMemoryPage::Map(VKNDevice* pDevice)
	{
		//If not mapped -> map
		if (!m_IsMapped)
		{
			void* pMemory = nullptr;
			vkMapMemory(pDevice->GetVkDevice(), m_DeviceMemory, 0, VK_WHOLE_SIZE, 0, &pMemory);

			m_pHostMemory   = reinterpret_cast<uint8*>(pMemory);
			m_IsMapped      = true;
		}
	}


	void VKNMemoryPage::Unmap(VKNDevice* pDevice)
	{
		//If mapped -> unmap
		if (m_IsMapped)
		{
			vkUnmapMemory(pDevice->GetVkDevice(), m_DeviceMemory);
			m_pHostMemory = nullptr;
			m_IsMapped    = false;
		}
	}


	void VKNMemoryPage::Deallocate(VKNAllocation& allocation)
	{
        
        //Try to find the correct block
        VKNMemoryBlock* pCurrent = allocation.pBlock;
		if (!pCurrent)
        {
            LOG_DEBUG_ERROR("Vulkan: Block owning allocation was not found\n");
            return;
        }

        LOG_DEBUG_INFO("Vulkan: Deallocating Block ID=%u\n", pCurrent->ID);
		
		//Set this block to free
		pCurrent->IsFree = true;

		//Merge previous with current
        if (pCurrent->pPrevious)
        {
            VKNMemoryBlock* pPrevious = pCurrent->pPrevious;
            if (pPrevious->IsFree)
            {
                //Set size
				pPrevious->SizeInBytes			+= pCurrent->SizeInBytes;
				pPrevious->PaddedSizeInBytes	+= pCurrent->PaddedSizeInBytes;

                //Set pointers
                pPrevious->pNext = pCurrent->pNext;
                if (pCurrent->pNext)
                    pCurrent->pNext->pPrevious = pPrevious;

                //Remove block
                delete pCurrent;
                pCurrent = pPrevious;
            }
        }
        
        //Try and merge current with next
        if (pCurrent->pNext)
        {
            VKNMemoryBlock* pNext = pCurrent->pNext;    
            if (pNext->IsFree)
            {
			   //Set size
			   pCurrent->SizeInBytes		+= pNext->SizeInBytes;
			   pCurrent->PaddedSizeInBytes	+= pNext->PaddedSizeInBytes;

                //Set pointers
                if (pNext->pNext)
                    pNext->pNext->pPrevious = pCurrent;
                pCurrent->pNext = pNext->pNext;

                //Remove block
                delete pNext;
            }
        }
	}


	void VKNMemoryPage::Destroy(VKNDevice* pDevice)
	{
		LAMBDA_ASSERT(pDevice != nullptr);

		if (m_DeviceMemory != VK_NULL_HANDLE)
		{
			//Unmap
			Unmap(pDevice);

			//Print memoryleaks
#if defined(LAMBDA_DEBUG)
			{
				VKNMemoryBlock* pDebug = m_pHead;
				LOG_DEBUG_WARNING("Allocated blocks left in MemoryPage %u:\n", m_ID);
				while (pDebug)
				{
					LOG_DEBUG_WARNING("    VulkanBlock: ID=%u, Offset=%u, Size=%u, IsFree=%s\n", pDebug->ID, pDebug->DeviceMemoryOffset, pDebug->SizeInBytes, pDebug->IsFree ? "True" : "False");
					pDebug = pDebug->pNext;
				}
			}
#endif

			//Delete all blocks
			VKNMemoryBlock* pCurrent = m_pHead;
			while (pCurrent != nullptr)
			{
				VKNMemoryBlock* pOld = pCurrent;
				pCurrent = pCurrent->pNext;

				//Delete block
				SafeDelete(pOld);
			}		

			//Free memory
			vkFreeMemory(pDevice->GetVkDevice(), m_DeviceMemory, nullptr);
			m_DeviceMemory = VK_NULL_HANDLE;

			LOG_SYSTEM(LOG_SEVERITY_WARNING, "Vulkan: Deallocated MemoryPage\n");
		}

		delete this;
	}

	//------------------
	//VKNDeviceAllocator
	//------------------

    constexpr size_t numFrames = 3;

	VKNDeviceAllocator::VKNDeviceAllocator(VKNDevice* pDevice)
		: m_pDevice(pDevice),
		m_MaxAllocations(0),
        m_TotalReserved(0),
        m_TotalAllocated(0),
        m_FrameIndex(0),
		m_Pages(),
		m_MemoryToDeallocate()
	{
        //Resize the number of garbage memory vectors
        m_MemoryToDeallocate.resize(numFrames);
        
        //Setup from properties of the device
		VkPhysicalDeviceProperties properties = pDevice->GetPhysicalDeviceProperties();
		m_MaxAllocations = properties.limits.maxMemoryAllocationCount;
        m_BufferImageGranularity = properties.limits.bufferImageGranularity;
	}


	VKNDeviceAllocator::~VKNDeviceAllocator()
	{
		//Cleanup all garbage memory before deleting
		for (uint32 i = 0; i < numFrames; i++)
			EmptyGarbageMemory();

		//Delete allocator
		LOG_SYSTEM(LOG_SEVERITY_WARNING, "Vulkan: Deleting DeviceAllocator. Number of Pages: %u\n", m_Pages.size());
		for (auto page : m_Pages)
		{
			if (page)
				page->Destroy(m_pDevice);
		}

		LOG_DEBUG_INFO("Vulkan: Destroyed DeviceAllocator\n");
	}


	bool VKNDeviceAllocator::Allocate(VKNAllocation& allocation, const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags properties)
	{
		//Add to total
		m_TotalAllocated += memoryRequirements.size;

		//Get needed memorytype
		uint32 memoryType = FindMemoryType(m_pDevice->GetVkPhysicalDevice(), memoryRequirements.memoryTypeBits, properties);

        
		//Try allocating from existing page
		for (auto page : m_Pages)
		{
			if (page->GetMemoryType() == memoryType)
			{
				if (page->Allocate(allocation, memoryRequirements.size, memoryRequirements.alignment, m_BufferImageGranularity))
				{
					LOG_DEBUG_WARNING("[VULKAN DEVICE ALLOCATOR] Allocated '%d' bytes. Memory-Type: %d. Total Allocated: %.2fMB. Total Reserved %.2fMB\n", memoryRequirements.size, memoryType, float(m_TotalAllocated) / mb, float(m_TotalReserved) / mb);
					return true;
				}
			}
		}
        
		LAMBDA_ASSERT_PRINT(m_Pages.size() < m_MaxAllocations, "Vulkan: Max number of allocations already reached\n");
		
		//If allocated is large, make a dedicated allocation
		uint64 bytesToReserve = MB(128);
		if (memoryRequirements.size > bytesToReserve)
			bytesToReserve = memoryRequirements.size;

		//Add to total
		m_TotalReserved += bytesToReserve;
		
		//Allocate new page
		VKNMemoryPage* pPage = DBG_NEW VKNMemoryPage(m_pDevice, uint32(m_Pages.size()), bytesToReserve, memoryType, properties);
		m_Pages.emplace_back(pPage);

		LOG_SYSTEM(LOG_SEVERITY_WARNING, "[VULKAN DEVICE ALLOCATOR] Allocated Memory-Page. Allocationcount: '%llu/%llu'. Memory-Type: %d. Total Allocated: %.2fMB. Total Reserved %.2fMB\n", m_Pages.size(), m_MaxAllocations, memoryType, float(m_TotalAllocated) / mb, float(m_TotalReserved) / mb);

        return pPage->Allocate(allocation, memoryRequirements.size, memoryRequirements.alignment, m_BufferImageGranularity);
	}


	void VKNDeviceAllocator::Deallocate(VKNAllocation& allocation)
	{
        //Set it to be removed
        if (allocation.pBlock && allocation.DeviceMemory != VK_NULL_HANDLE)
            m_MemoryToDeallocate[m_FrameIndex].emplace_back(allocation);

        //Invalidate memory
		allocation.pBlock			  = nullptr;
        allocation.DeviceMemoryOffset = 0;
        allocation.SizeInBytes		  = 0;
        allocation.DeviceMemory       = VK_NULL_HANDLE;
        allocation.pHostMemory        = nullptr;
	}


	void VKNDeviceAllocator::EmptyGarbageMemory()
	{
        //Move on a frame
        m_FrameIndex = (m_FrameIndex+1) % numFrames;
        
        //Clean memory
		auto& memoryBlocks = m_MemoryToDeallocate[m_FrameIndex];
		if (!memoryBlocks.empty())
		{
            //Deallocate all the blocks
			for (auto& memory : memoryBlocks)
            {
                if (memory.pBlock && memory.DeviceMemory != VK_NULL_HANDLE)
                {
                    VKNMemoryPage* pPage = memory.pBlock->pPage;
                    pPage->Deallocate(memory);
                    
                    m_TotalAllocated -= memory.SizeInBytes;

					LOG_DEBUG_WARNING("[VULKAN DEVICE ALLOCATOR] Deallocated '%d' bytes. Total Allocated: %.2fMB. Total Reserved %.2fMB\n", memory.SizeInBytes, float(m_TotalAllocated) / mb, float(m_TotalReserved) / mb);
                }
            }
            
			memoryBlocks.clear();
		}


		//Remove empty pages
		if (m_Pages.size() > 6)
		{
			for (auto it = m_Pages.begin(); it != m_Pages.end();)
			{
				if ((*it)->IsEmpty())
				{
					//Erase from vector
					m_TotalReserved -= (*it)->GetSize();

					(*it)->Destroy(m_pDevice);
					it = m_Pages.erase(it);

					LOG_SYSTEM(LOG_SEVERITY_WARNING, "[VULKAN DEVICE ALLOCATOR] Destroyed Memory-Page. Allocationcount: '%llu/%llu'. Total Allocated: %.2fMB. Total Reserved %.2fMB\n", m_Pages.size(), m_MaxAllocations, float(m_TotalAllocated) / mb, float(m_TotalReserved) / mb);
				}
				else
				{
					it++;
				}
			}
		}
	}
}
