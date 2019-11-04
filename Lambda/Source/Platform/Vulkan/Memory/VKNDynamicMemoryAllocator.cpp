#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VKNDynamicMemoryAllocator.h"
#include "../VKNDevice.h"

//#define LAMBDA_DYNAMIC_ALLOCATOR_DEBUG
#if defined(LAMBDA_DYNAMIC_ALLOCATOR_DEBUG)
	#define LAMBDA_DYNAMIC_ALLOCATOR_DEBUG_ALLOC
	#define LAMBDA_DYNAMIC_ALLOCATOR_DEBUG_DEALLOC
#endif

namespace Lambda
{
	//---------------------
	//VKNDynamicMemoryPage
	//---------------------

	VKNDynamicMemoryPage::VKNDynamicMemoryPage(VKNDevice* pVkDevice, uint32 id, VkDeviceSize sizeInBytes)
		: m_VkBuffer(VK_NULL_HANDLE),
		m_ID(id),
		m_SizeInBytes(sizeInBytes),
		m_BlockCount(0),
		m_pHead(nullptr),
		m_pNextFree(nullptr),
		m_VkBlockPool(sizeInBytes / 64) 
	{
		Init(pVkDevice);
	}


	void VKNDynamicMemoryPage::Init(VKNDevice* pVkDevice)
	{
		//Create buffer
		VkBufferCreateInfo info = {};
		info.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.pNext					= nullptr;
		info.flags					= 0;
		info.size					= m_SizeInBytes;
		info.queueFamilyIndexCount	= 0;
		info.pQueueFamilyIndices	= nullptr;
		info.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
		info.usage					=	VK_BUFFER_USAGE_TRANSFER_DST_BIT  |
										VK_BUFFER_USAGE_TRANSFER_SRC_BIT  |
										VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
										VK_BUFFER_USAGE_INDEX_BUFFER_BIT  |
										VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		if (vkCreateBuffer(pVkDevice->GetVkDevice(), &info, nullptr, &m_VkBuffer) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Failed to create Buffer\n");
			return;
		}
		else
		{
			LOG_DEBUG_WARNING("[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Created Page\n");
		}

		//Memory properties
		VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		//Allocate memory
		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(pVkDevice->GetVkDevice(), m_VkBuffer, &memoryRequirements);
		if (!pVkDevice->Allocate(m_Memory, memoryRequirements, memoryProperties))
		{
			LOG_DEBUG_ERROR("[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Failed to allocate page '%p'\n", m_VkBuffer);
			return;
		}
		else
		{
			LOG_DEBUG_WARNING("[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Allocated '%d' bytes for page\n", m_SizeInBytes);
			if (vkBindBufferMemory(pVkDevice->GetVkDevice(), m_VkBuffer, m_Memory.DeviceMemory, m_Memory.DeviceMemoryOffset) != VK_SUCCESS)
			{
				LOG_DEBUG_WARNING("[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Failed to bind Buffer-Memory\n", m_SizeInBytes);
			}
		}

		//Set name
		pVkDevice->SetVulkanObjectName(VK_OBJECT_TYPE_BUFFER, (uint64)m_VkBuffer, "Dynamic Memory Page [" + std::to_string(m_ID) + "]");

		//Setup first block
		m_pHead					= m_VkBlockPool.Get();
		m_pHead->pPage			= this;
		m_pHead->pNext			= nullptr;
		m_pHead->pPrevious		= nullptr;
		m_pHead->IsFree			= true;
		m_pHead->SizeInBytes	= m_SizeInBytes;
		m_pHead->BufferOffset	= 0;
#if defined(LAMBDA_DEBUG)
		m_pHead->ID = m_BlockCount++;
#endif
		//Set next free to the first block
		m_pNextFree = m_pHead;
	}


	bool VKNDynamicMemoryPage::Allocate(VKNDynamicAllocation& allocation, VkDeviceSize sizeInBytes, VkDeviceSize alignment)
	{
		VkDeviceSize paddedBufferOffset = 0;
		VkDeviceSize paddedSizeInBytes	= 0;
		VKNDynamicMemoryBlock* pBestFit = nullptr;

		//Find enough free space, and find the block that best fits
		for (VKNDynamicMemoryBlock* pCurrent = m_pNextFree; pCurrent != nullptr; pCurrent = pCurrent->pNext)
		{
			//Check if the block is allocated or not
			if (!pCurrent->IsFree)
				continue;

			//Align the offset and padding
			paddedBufferOffset = Math::AlignUp<uint64>(pCurrent->BufferOffset, alignment);
			paddedSizeInBytes = sizeInBytes + (paddedBufferOffset - pCurrent->BufferOffset);

			//Does it fit into the block
			if (paddedSizeInBytes > pCurrent->SizeInBytes)
				continue;

			pBestFit = pCurrent;
			break;
		}


		//If we did not find a fitting block we try again but from the start of the list
		if (pBestFit == nullptr)
		{
			m_pNextFree = m_pHead;
			for (VKNDynamicMemoryBlock* pCurrent = m_pNextFree; pCurrent != nullptr; pCurrent = pCurrent->pNext)
			{
				//Check if the block is allocated or not
				if (!pCurrent->IsFree)
					continue;

				//Align the offset and padding
				paddedBufferOffset = Math::AlignUp<uint64>(pCurrent->BufferOffset, alignment);
				paddedSizeInBytes = sizeInBytes + (paddedBufferOffset - pCurrent->BufferOffset);

				//Does it fit into the block
				if (paddedSizeInBytes > pCurrent->SizeInBytes)
					continue;

				pBestFit = pCurrent;
				break;
			}
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
			VKNDynamicMemoryBlock* pBlock = m_VkBlockPool.Get();
			pBlock->pPage			= this;
			pBlock->SizeInBytes		= pBestFit->SizeInBytes - paddedSizeInBytes;
			pBlock->BufferOffset	= paddedBufferOffset + sizeInBytes;
			pBlock->IsFree			= true;
#if defined(LAMBDA_DEBUG)
			pBlock->ID				= m_BlockCount++;
#endif

			//Set pointers
			pBlock->pNext		= pBestFit->pNext;
			pBlock->pPrevious	= pBestFit;
			if (pBestFit->pNext)
				pBestFit->pNext->pPrevious = pBlock;
			pBestFit->pNext = pBlock;

			//Set block as the next free block. At next allocation we start by looking at this block
			m_pNextFree = pBlock;
		}
		else
		{
			m_pNextFree = m_pHead;
		}

		//Update bestfit
		pBestFit->SizeInBytes	= paddedSizeInBytes;
		pBestFit->IsFree		= false;

		//Setup allocation
		allocation.pBlock		= pBestFit;
		allocation.Buffer		= m_VkBuffer;
		allocation.BufferOffset = paddedBufferOffset;
		allocation.pHostMemory	= m_Memory.pHostMemory + allocation.BufferOffset;

#if defined (LAMBDA_DYNAMIC_ALLOCATOR_DEBUG_ALLOC)
		{
			LOG_DEBUG_INFO("[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Page '%d'\n", m_ID);
			for (VKNDynamicMemoryBlock* pCurrent = m_pHead; pCurrent != nullptr; pCurrent = pCurrent->pNext)
			{
				LOG_DEBUG_INFO("----Block %d----\n", pCurrent->ID);
				LOG_DEBUG_INFO("Starts at: %llu\n", pCurrent->BufferOffset);
				LOG_DEBUG_INFO("Free: %s\n", pCurrent->IsFree ? "true" : "false");
				if (pCurrent->pPrevious)
				{
					VKNDynamicMemoryBlock* pPrevious = pCurrent->pPrevious;
					if ((pPrevious->BufferOffset + pPrevious->PaddedSizeInBytes) > pCurrent->BufferOffset)
					{
						LOG_DEBUG_WARNING("[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Overlapping memory in page '%d' between blocks '%d' and '%d'\n", m_ID, pPrevious->ID, pCurrent->ID);
					}
				}
				LOG_DEBUG_INFO("   End at: %llu\n", pCurrent->BufferOffset + pCurrent->PaddedSizeInBytes);
				LOG_DEBUG_INFO("----------------\n", pCurrent->ID);
			}
		}
#endif

		return true;
	}


	void VKNDynamicMemoryPage::Deallocate(VKNDynamicAllocation& allocation)
	{
		//LOG_DEBUG_INFO("Vulkan: Deallocated Dynamic-Block ID=%u\n", allocation.BlockID);

		//Try to find the correct block
		VKNDynamicMemoryBlock* pCurrent = allocation.pBlock;
		if (!pCurrent)
		{
			LOG_DEBUG_ERROR("[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Block for allocation is invalid\n");
			return;
		}

		//Set this block to free
		pCurrent->IsFree = true;

		//Merge previous with current
		if (pCurrent->pPrevious)
		{
			VKNDynamicMemoryBlock* pPrevious = pCurrent->pPrevious;
			if (pPrevious->IsFree)
			{
				//Set size
				pPrevious->SizeInBytes += pCurrent->SizeInBytes;

				//Set pointers
				pPrevious->pNext = pCurrent->pNext;
				if (pCurrent->pNext)
					pCurrent->pNext->pPrevious = pPrevious;

				//Remove block
				m_VkBlockPool.Return(pCurrent);
				pCurrent = pPrevious;
			}
		}

		//Try and merge current with next
		if (pCurrent->pNext)
		{
			VKNDynamicMemoryBlock* pNext = pCurrent->pNext;
			if (pNext->IsFree)
			{
				//Set size
				pCurrent->SizeInBytes += pNext->SizeInBytes;

				//Set pointers
				if (pNext->pNext)
					pNext->pNext->pPrevious = pCurrent;
				pCurrent->pNext = pNext->pNext;

				//Remove block
				m_VkBlockPool.Return(pNext);
			}
		}

#if defined (LAMBDA_DYNAMIC_ALLOCATOR_DEBUG_DEALLOC)
		{
			LOG_DEBUG_INFO("[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Page '%d'\n", m_ID);
			for (pCurrent = m_pHead; pCurrent != nullptr; pCurrent = pCurrent->pNext)
			{
				LOG_DEBUG_INFO("----Block %d----\n", pCurrent->ID);
				LOG_DEBUG_INFO("Starts at: %llu\n", pCurrent->BufferOffset);
				LOG_DEBUG_INFO("Free: %s\n", pCurrent->IsFree ? "true" : "false");
				if (pCurrent->pPrevious)
				{
					VKNDynamicMemoryBlock* pPrevious = pCurrent->pPrevious;
					if ((pPrevious->BufferOffset + pPrevious->PaddedSizeInBytes) > pCurrent->BufferOffset)
					{
						LOG_DEBUG_WARNING("[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Overlapping memory in page '%d' between blocks '%d' and '%d'\n", m_ID, pPrevious->ID, pCurrent->ID);
					}
				}
				LOG_DEBUG_INFO("   End at: %llu\n", pCurrent->BufferOffset + pCurrent->PaddedSizeInBytes);
				LOG_DEBUG_INFO("----------------\n", pCurrent->ID);
			}
		}
#endif
	}


	void VKNDynamicMemoryPage::Destroy(VKNDevice* pVkDevice)
	{
		LAMBDA_ASSERT(pVkDevice != nullptr);

		//Print memoryleaks
#if defined(LAMBDA_DEBUG)
		VKNDynamicMemoryBlock* pDebug = m_pHead;
		LOG_DEBUG_WARNING("[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Allocated blocks left in page %u:\n", m_ID);
		while (pDebug)
		{
			LOG_DEBUG_WARNING("    DynamicBlock: ID=%u, Offset=%u, Size=%u, IsFree=%s\n", pDebug->ID, pDebug->BufferOffset, pDebug->SizeInBytes, pDebug->IsFree ? "True" : "False");
			pDebug = pDebug->pNext;
		}
#endif

		//Deallocate memory from global memory manager
		pVkDevice->Deallocate(m_Memory);

		//Delete first block
		m_VkBlockPool.Return(m_pHead);

		//Delete buffer
		if (m_VkBuffer != VK_NULL_HANDLE)
			pVkDevice->SafeReleaseVkResource<VkBuffer>(m_VkBuffer);

		LOG_SYSTEM(LOG_SEVERITY_WARNING, "[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Deallocated page\n");
		delete this;
	}


	//-------------------------
	//VKNDynamicMemoryAllocator
	//-------------------------

	constexpr size_t numFrames = 3;
	constexpr uint64 pageSize = MB(32);

	VKNDynamicMemoryAllocator::VKNDynamicMemoryAllocator(VKNDevice* pVkDevice)
		: m_pVkDevice(pVkDevice),
		m_pCurrentPage(nullptr),
		m_TotalReserved(0),
		m_TotalAllocated(0),
		m_FrameIndex(0),
		m_Pages(),
        m_EmptyPages(),
		m_MemoryToDeallocate()
	{
		//Create first page
		m_pCurrentPage = DBG_NEW VKNDynamicMemoryPage(m_pVkDevice, uint32(m_Pages.size()), pageSize);
		m_Pages.emplace_back(m_pCurrentPage);

		//Resize the number of garbage memory vectors
		m_MemoryToDeallocate.resize(numFrames);
		for (auto& mem : m_MemoryToDeallocate)
			mem.reserve(pageSize / 64);
	}


	VKNDynamicMemoryAllocator::~VKNDynamicMemoryAllocator()
	{
		//Cleanup all garbage memory before deleting
		for (uint32 i = 0; i < numFrames; i++)
			EmptyGarbageMemory();

		//Delete allocator
		LOG_SYSTEM(LOG_SEVERITY_WARNING, "[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Deleting allocator. Number of pages: %u\n", m_Pages.size());
		for (auto page : m_Pages)
		{
			if (page)
				page->Destroy(m_pVkDevice);
		}

		LOG_DEBUG_INFO("[Vulkan] [DYNAMIC MEMORY ALLOCATOR] Destroyed allocator\n");
	}


	bool VKNDynamicMemoryAllocator::Allocate(VKNDynamicAllocation& allocation, uint64 sizeInBytes, uint64 alignment)
	{
		//Add to total
		m_TotalAllocated += sizeInBytes;

		//Try allocating from existing page
		LAMBDA_ASSERT(m_pCurrentPage != nullptr);

		if (m_pCurrentPage->Allocate(allocation, sizeInBytes, alignment))
		{
			return true;
		}
		else
		{
			for (auto page : m_Pages)
			{
				if (page->Allocate(allocation, sizeInBytes, alignment))
				{
					m_pCurrentPage = page;
					return true;
				}
			}
		}
        

        //Add to total
        uint64 bytesToReserve = pageSize;
        m_TotalReserved += bytesToReserve;

        //Only allocate a new page if necessary, by now we should have found a page with enough space in case there are any empty pages
		m_pCurrentPage = DBG_NEW VKNDynamicMemoryPage(m_pVkDevice, uint32(m_Pages.size()), bytesToReserve);
		m_Pages.emplace_back(m_pCurrentPage);

		return m_pCurrentPage->Allocate(allocation, sizeInBytes, alignment);
	}


	void VKNDynamicMemoryAllocator::Deallocate(VKNDynamicAllocation& allocation)
	{
		//Set it to be removed
		if (allocation.pBlock && allocation.Buffer != VK_NULL_HANDLE)
			m_MemoryToDeallocate[m_FrameIndex].emplace_back(allocation);

		//Invalidate memory
		memset(&allocation, 0, sizeof(VKNDynamicAllocation));
	}


	void VKNDynamicMemoryAllocator::EmptyGarbageMemory()
	{
		//Move on a frame
		m_FrameIndex = (m_FrameIndex + 1) % numFrames;

		//Deallocate memory
		auto& memoryBlocks = m_MemoryToDeallocate[m_FrameIndex];
		if (!memoryBlocks.empty())
		{
			for (auto& memory : memoryBlocks)
			{
				m_TotalAllocated -= memory.pBlock->SizeInBytes;
				
				VKNDynamicMemoryPage* pPage = memory.pBlock->pPage;
				pPage->Deallocate(memory);
			}

			memoryBlocks.clear();
		}

        
        //Go through all the pages and find the empty ones (Prepare for next frame)
        m_EmptyPages.clear();
        for (auto page : m_Pages)
        {
            if (page->IsEmpty())
                m_EmptyPages.emplace_back(page);
        }
        
        
		//If we have more than "limit"-number or pages, we check for empty ones we can remove
		constexpr size_t limit = 4;
		if (m_EmptyPages.size() > limit)
		{
            //Calculate pages to remove
            uint32 numToRemove = m_EmptyPages.size() - limit;
			
            //Remove empty pages
            for (uint32 i = 0; i < numToRemove; i++)
			{
                //Destroy the last page and then pop it. We repeat this numToRemove number of times
                m_EmptyPages.back()->Destroy(m_pVkDevice);
                m_EmptyPages.pop_back();
			}
		}
	}
	
	//-------------------------------
	//VKNDynamicMemoryPage::BlockPool
	//-------------------------------

	VKNDynamicMemoryPage::VKNBlockPool::VKNBlockPool(uint32 numBlocks)
		: m_pHead(nullptr),
		m_Chains()
	{
		m_pHead = AllocateBlocks(numBlocks);
	}


	VKNDynamicMemoryPage::VKNBlockPool::~VKNBlockPool()
	{
		for (auto chain : m_Chains)
			SafeDeleteArr(chain);
	}
	
	
	VKNDynamicMemoryBlock* VKNDynamicMemoryPage::VKNBlockPool::Get()
	{
		VKNDynamicMemoryBlock* pFirst = m_pHead;
		m_pHead = m_pHead->pNext;
		if (m_pHead)
		{
			//Remove previous from new head
			m_pHead->pPrevious = nullptr;
		}
		else
		{
			//If we have taken the last block, allocate new ones
			m_pHead = AllocateBlocks(4096);
		}

		memset(pFirst, 0, sizeof(VKNDynamicMemoryBlock));
		return pFirst;
	}
	
	
	void VKNDynamicMemoryPage::VKNBlockPool::Return(VKNDynamicMemoryBlock* pBlock)
	{
		pBlock->pNext		= m_pHead;
		pBlock->pPrevious	= nullptr;
		m_pHead->pPrevious	= pBlock;
		m_pHead				= pBlock;
	}
	
	
	VKNDynamicMemoryBlock* VKNDynamicMemoryPage::VKNBlockPool::AllocateBlocks(uint32 numBlocks)
	{
		//Allocate array of blocks
		VKNDynamicMemoryBlock* pBlocks = DBG_NEW VKNDynamicMemoryBlock[numBlocks];
		m_Chains.emplace_back(pBlocks);
		
		//Build a chain of them
		VKNDynamicMemoryBlock* pCurrent	 = pBlocks;
		VKNDynamicMemoryBlock* pFirst	 = pCurrent;
		VKNDynamicMemoryBlock* pPrevious = nullptr;
		for (uint32 i = 1; i < numBlocks; i++)
		{
			pCurrent->pNext = &pBlocks[i];
			if (pPrevious)
				pCurrent->pPrevious = pPrevious;
			
			pPrevious = pCurrent;
			pCurrent = pCurrent->pNext;
		}

		return pFirst;
	}
}
